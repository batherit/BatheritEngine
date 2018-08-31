#include<algorithm>
#include "CSteeringBehaviors.h"
#include "CObjectFuncTemplates.h"
#include"CUtil.h"
#include "CGameTimer.h"
#include "CGameObject.h"
#include "CVehicle.h"
using namespace std;

CSteeringBehavior::CSteeringBehavior(CVehicle* agent) :
	p_vehicle_(agent),
	behavior_flags_(0),
	weight_seek_(VehiclePrm.seek_weight_),
	weight_flee_(VehiclePrm.flee_weight_),
	weight_arrive_(VehiclePrm.arrive_weight_),
	weight_pursuit_(VehiclePrm.pursuit_weight_),
	weight_offset_pursuit_(VehiclePrm.offset_pursuit_weight_),
	weight_evade_(VehiclePrm.evade_weight_),
	weight_wander_(VehiclePrm.wander_weight_),
	weight_obstacle_avoidance_(VehiclePrm.obstacle_avoidance_weight_),
	weight_wall_avoidance_(VehiclePrm.wall_avoidance_weight_),
	weight_follow_path_(VehiclePrm.follow_path_weight_),
	view_distance_(VehiclePrm.view_distance_),
	wall_detection_feeler_length_(VehiclePrm.wall_detection_feeler_length_),
	feelers_(3),
	deceleration_(normal),
	wander_distance_(WANDER_DIST),
	wander_jitter_(WANDER_JITTER_PER_SEC),
	wander_radius_(WANDER_RAD),
	waypoint_seek_dist_sq_(WAY_POINT_SEEK_DIST * WAY_POINT_SEEK_DIST),
	p_target_agent1_(nullptr),
	p_target_agent2_(nullptr)
{
	float theta = RandFloat() * TwoPi;
	wander_target_ = CVector2D(wander_radius_ * cos(theta), wander_radius_ * sin(theta));

	p_path_ = new CPath2D();
	p_path_->LoopOn();
}

CVector2D CSteeringBehavior::Calculate() {
	v_steering_force_.Zero();

	// TODO : 공간 분할 설정
	// TODO : 조종 행동 유형 변경
	v_steering_force_ = CalculateWeightedSum();

	return v_steering_force_;
}

void CSteeringBehavior::CreateFeelers() {
	//feeler pointing straight in front
	feelers_[0] = p_vehicle_->transform_.pos_ + wall_detection_feeler_length_ * p_vehicle_->transform_.look_;

	//feeler to left
	CVector2D temp = p_vehicle_->transform_.look_;
	Vec2DRotateAroundOrigin(temp, HalfPi * 3.5f);
	feelers_[1] = p_vehicle_->transform_.pos_ + wall_detection_feeler_length_ / 2.0f * temp;

	//feeler to right
	temp = p_vehicle_->transform_.look_;
	Vec2DRotateAroundOrigin(temp, HalfPi * 0.5f);
	feelers_[2] = p_vehicle_->transform_.pos_ + wall_detection_feeler_length_ / 2.0f * temp;
}

//this behavior moves the agent towards a target position
CVector2D CSteeringBehavior::Seek(CVector2D target_pos) {
	CVector2D desired_velocity = Vec2DNormalize(target_pos - p_vehicle_->transform_.pos_)
		* p_vehicle_->physics_->MaxSpeed();

	return (desired_velocity - p_vehicle_->physics_->Velocity());
}

//this behavior returns a vector that moves the agent away
//from a target position
CVector2D CSteeringBehavior::Flee(CVector2D target_pos) {
	
	// 패닉 상태(제곱 거리 판별)에 있을 때에만 '달아나기(Flee)'가 유효하도록 한다.
	const double panic_distance_sq = 100.0f * 100.0f;
	if (Vec2DDistanceSq(p_vehicle_->transform_.pos_, target_pos) > panic_distance_sq) {
		return CVector2D(0,0);
	}
	

	CVector2D desired_velocity = Vec2DNormalize(p_vehicle_->transform_.pos_ - target_pos)
		* p_vehicle_->physics_->MaxSpeed();

	return (desired_velocity - p_vehicle_->physics_->Velocity());
}

// 목표에 도달하기 위한 가감속이 있는 '찾기(Seek)'행동이라 보면 될 것이다.
CVector2D CSteeringBehavior::Arrive(CVector2D     TargetPos, Deceleration deceleration) {
	CVector2D to_target = TargetPos - p_vehicle_->transform_.pos_;

	//calculate the distance to the target
	float dist = to_target.Length();

	if (dist > 0)
	{
		// Deceleration(감속)이 열거된 정수이므로, 미세조정된 감속을
		// 제공하기 위해 이 값이 요구된다.
		const float deceleration_tweaker = 0.3f;

		// 원하는 감속이 주어진 경우 목표에 도달하기 위해 요구되는 속도를 계산한다.
		float speed = dist / ((float)deceleration * deceleration_tweaker);

		// 속도가 최대값이 넘지 않는지를 확인한다.
		speed = min(speed, p_vehicle_->physics_->MaxSpeed());

		// 여기서부터는 이미 to_target 벡터의 길이, dist를 계산했으므로
		// 이 벡터를 정규화할 필요가 없다는 것을 제외하고는
		// 찾기와 같은 방법으로 진행한다.
		CVector2D desired_velocity = to_target * speed / dist;

		return (desired_velocity - p_vehicle_->physics_->Velocity());
	}

	return CVector2D(0, 0);
}

// '추격하기(Pursuit)'은 에이전트의 미래 위치를 예측하여 그 지점으로 이동한다.
// 목표물을 가로챌 때 유용하다.
CVector2D CSteeringBehavior::Pursuit(const CVehicle* evader) {
	// 만약 도피자가 앞에 있고 에이전트를 대면하고 있다면, 도피자의
	// 현재 위치만을 찾을 수 있다.
	CVector2D to_evader = evader->transform_.pos_ - p_vehicle_->transform_.pos_;

	float relative_look = p_vehicle_->transform_.look_.Dot(evader->transform_.look_);

	if ((to_evader.Dot(p_vehicle_->transform_.look_) > 0) &&
		(relative_look < -0.95))  //acos(0.95)=18 degs
	{
		return Seek(evader->transform_.pos_);
	}

	// 바로 앞에 있지 않다고 간주되면, 도피자가 어디 있을 것인지를 예측한다.

	// 예측 시간은 도피자와 추격자 사이의 거리에 비례하고,
	// 이 에이전트들의 속도의 합에는 반비례한다.
	float look_ahead_time = to_evader.Length() /
		(p_vehicle_->physics_->MaxSpeed() + evader->physics_->Speed());

	// 이제 도피자가 있을 것으로 예측된 미래 위치로 찾아간다.
	return Seek(evader->transform_.pos_ + evader->physics_->Velocity() * look_ahead_time);
}

//this behavior attempts to evade a pursuer
CVector2D CSteeringBehavior::Evade(const CVehicle* pursuer) {
	/* Not necessary to include the check for facing direction this time */

	CVector2D ToPursuer = pursuer->transform_.pos_ - p_vehicle_->transform_.pos_;

	//uncomment the following two lines to have Evade only consider pursuers 
	//within a 'threat range'
	const float ThreatRange = 100.0;
	if (ToPursuer.LengthSq() > ThreatRange * ThreatRange) return CVector2D();

	//the lookahead time is propotional to the distance between the pursuer
	//and the pursuer; and is inversely proportional to the sum of the
	//agents' velocities
	float LookAheadTime = ToPursuer.Length() /
		(p_vehicle_->physics_->MaxSpeed() + pursuer->physics_->Speed());

	//now flee away from predicted future position of the pursuer
	return Flee(pursuer->transform_.pos_ + pursuer->physics_->Velocity() * LookAheadTime);
}

//this behavior makes the agent wander about randomly
CVector2D CSteeringBehavior::Wander() {
	/*this behavior is dependent on the update rate, so this line must
	be included when using time independent framerate.*/
	float jitter_this_time_slice = wander_jitter_ * WorldTimer->GetElapsedTimePerFrame();  /*wander_jitter_ * WorldTimer*/

	// 우선 소량의 무작위 벡터를 목표물의 위치에 더한다. (RandomClamped는
	// -1과 1 사이의 값을 반환한다.
	wander_target_ += CVector2D(RandomClamped() * jitter_this_time_slice,
		RandomClamped() * jitter_this_time_slice);

	// 이 새로운 벡터를 다시 단위원으로 재투사한다.
	wander_target_.Normalize();

	// 벡터의 길이를 배회 원의 반경과 동일하게 증가시킨다.
	wander_target_ *= wander_radius_;

	// 목표를 에이전트 앞의 wander_distance_ 위치로 이동시킨다.
	CVector2D target_local = wander_target_ + CVector2D(wander_distance_, 0);

	// 목표를 세계 공간으로 투사한다. Local -> World
	CVector2D target_world = PointToWorldSpace(target_local,
		p_vehicle_->transform_.look_,
		p_vehicle_->transform_.right_,
		p_vehicle_->transform_.pos_);

	// 그리고 그쪽으로 조종해간다.
	return target_world - p_vehicle_->transform_.pos_;
}

CVector2D CSteeringBehavior::ObstacleAvoidance(const std::vector<CGameObject*>& obstacles) {
	// 감지 상자의 길이는 에이전트의 속도에 비례한다.
	detection_box_length_ = VehiclePrm.min_detection_box_length_ +
		(p_vehicle_->physics_->Speed() / p_vehicle_->physics_->MaxSpeed()) *
		VehiclePrm.min_detection_box_length_;

	// 감지 상자 범위 내의 장애물들을 처리하기 위해 표시해둔다.
	p_vehicle_->GameWorld()->TagObstaclesWithinViewRange(p_vehicle_, detection_box_length_);

	// 이 부분은 교차하고 있는 가장 근접한 장애물(CIB)들을 추적한다.
	CGameObject* closest_intersecting_obstacle = nullptr;

	// 이 부분은 CIB까지의 거리를 추적하는데 사용된다.
	float dist_to_closest_IP = MaxFloat;
	CVector2D local_pos_of_closest_obstacle;
	
	std::vector<CGameObject*>::const_iterator curOb = obstacles.begin();

	while (curOb != obstacles.end())
	{
		// 장애물이 범위 내 태그된 경우 진행한다.
		if ((*curOb)->IsTagged())
		{
			// 비히클의 로컬 공간에서 장애물의 위치를 계산한다.
			CVector2D local_pos = PointToLocalSpace((*curOb)->transform_.pos_,
				p_vehicle_->transform_.look_,
				p_vehicle_->transform_.right_,
				p_vehicle_->transform_.pos_);


			// 로컬 위치가 음수 x 값이면 에이전트 뒤에 있어야 한다. (이 경우 무시할 수 있음)
			if (local_pos.x_ >= 0)
			{

				// x 축에서 객체의 위치까지의 거리가 반경 + 감지 상자의 너비의 절반보다 작으면 
				// 잠재적인 교차점이라 할 수 있다.
				float expanded_radius =
					(*curOb)->Mesh()->GetBoundingRad() +
					p_vehicle_->Mesh()->GetBoundingRad();

				if (fabs(local_pos.y_) < expanded_radius)
				{
					// 이제 선 / 원 교차 테스트를 수행한다. 원의 중심은(cX, cY)로 표시된다.
					// 교점은 y = 0에 대한 공식 x = cX + / -sqrt(r ^ 2 - cY ^ 2)로 표시된다.
					// x의 가장 작은 양의 값은 가장 가까운 교차점이 될 것이다.
					float cX = local_pos.x_;
					float cY = local_pos.y_;

					// 위 방정식의 sqrt 부분을 한 번만 계산하면 된다.
					float sqrt_part = sqrt(expanded_radius*expanded_radius - cY * cY);

					float ip = cX - sqrt_part;

					if (ip <= 0.0f)
					{
						ip = cX + sqrt_part;
					}

					// 장애물과 그 지역 좌표를 기록한다면, 이것이 가장 가까운 것인지를 판단한다. 
					if (ip < dist_to_closest_IP)
					{
						dist_to_closest_IP = ip;

						closest_intersecting_obstacle = *curOb;

						local_pos_of_closest_obstacle = local_pos;
					}
				}
			}
		}

		++curOb;
	}

	// 교차하는 장애물을 발견했다면, 그것으로부터 조향 힘을 계산한다.
	CVector2D steering_force;

	if (closest_intersecting_obstacle)
	{
		// 에이전트가 객체에 가까울수록 조향력이 강해야 한다.
		float multiplier = 1.0f + (detection_box_length_ - local_pos_of_closest_obstacle.x_) /
			detection_box_length_;

		// 횡력(lateral force)을 계산한다.
		steering_force.y_ = (closest_intersecting_obstacle->Mesh()->GetBoundingRad() -
			local_pos_of_closest_obstacle.y_)  * multiplier;

		// 차량으로부터의 장애물 거리에 비례하는 제동력을가한다.
		const float braking_weight = 0.2f;

		steering_force.x_ = (closest_intersecting_obstacle->Mesh()->GetBoundingRad() -
			local_pos_of_closest_obstacle.x_) *
			braking_weight;
	}

	// 마지막으로, 조향력을 로컬 공간에서 월드 공간으로 변환한다.
	return VectorToWorldSpace(steering_force,
		p_vehicle_->transform_.look_,
		p_vehicle_->transform_.right_);
}

CVector2D CSteeringBehavior::WallAvoidance(const std::vector<CWall2D> &walls) {
	//the feelers are contained in a std::vector, m_Feelers
	CreateFeelers();

	float dist_to_this_IP = 0.0f;
	float dist_to_closest_IP = MaxFloat;

	//this will hold an index into the vector of walls
	int closest_wall = -1;

	CVector2D steering_force,
		point,         //used for storing temporary info
		closest_point;  //holds the closest intersection point

					   //examine each feeler in turn
	for (unsigned int flr = 0; flr<feelers_.size(); ++flr)
	{
		//run through each wall checking for any intersection points
		for (unsigned int w = 0; w<walls.size(); ++w)
		{
			if (LineIntersection2D(p_vehicle_->transform_.pos_,
				feelers_[flr],
				walls[w].From(),
				walls[w].To(),
				dist_to_this_IP,
				point))
			{
				//is this the closest found so far? If so keep a record
				if (dist_to_this_IP < dist_to_closest_IP)
				{
					dist_to_closest_IP = dist_to_this_IP;

					closest_wall = w;

					closest_point = point;
				}
			}
		}//next wall


		 //if an intersection point has been detected, calculate a force  
		 //that will direct the agent away
		if (closest_wall >= 0)
		{
			//calculate by what distance the projected position of the agent
			//will overshoot the wall
			CVector2D over_shoot = feelers_[flr] - closest_point;

			//create a force in the direction of the wall normal, with a 
			//magnitude of the overshoot
			steering_force = walls[closest_wall].Normal() * over_shoot.Length();
		}

	}//next feeler

	return steering_force;
}

//------------------------------- FollowPath -----------------------------
//
// 일련의 벡터 집합(웨이 포인트)을 가지고서, 이 메서드는 순서대로 웨이포인트를 따라 에이전트를 움직이는
//  힘을 생성한다. 에이전트를 다음 웨이 포인트로 이동하기 위하여 'Seek'행동을
// 사용한다. 마지막 웨이 포인트에 대해선 'Arrive'행동을 사용한다.
//------------------------------------------------------------------------
CVector2D CSteeringBehavior::FollowPath()
{
	// 현재 목표에 충분히 근접하면 다음 목표로 이동
	// (거리 제곱 공간에서 작업)
	if (Vec2DDistanceSq(p_path_->CurrentWaypoint(), p_vehicle_->transform_.pos_) <
		waypoint_seek_dist_sq_)
	{
		p_path_->SetNextWaypoint();
	}

	if (!p_path_->Finished())
	{
		return Seek(p_path_->CurrentWaypoint());
	}

	else
	{
		return Arrive(p_path_->CurrentWaypoint(), normal);
	}
}

//---------------------- CalculateWeightedSum ----------------------------
//
// 이것은 단순히 모든 '활성화된 동작들 * 가중치'를 합하고 
// 결과를 반환하기 전에 최대로 이용할 수 있는 조종힘으로 수치를 자릅니다.
//------------------------------------------------------------------------
CVector2D CSteeringBehavior::CalculateWeightedSum() {
	if (On(wall_avoidance)) {
		v_steering_force_ += WallAvoidance(p_vehicle_->GameWorld()->Walls()) * weight_wall_avoidance_;
	}

	if (On(obstacle_avoidance)) {
		v_steering_force_ += ObstacleAvoidance(p_vehicle_->GameWorld()->Obstacles()) * weight_obstacle_avoidance_;
	}

	CVector2D pick_point = p_vehicle_->GameWorld()->PickPoint();

	if (On(seek))
	{
		v_steering_force_ += Seek(pick_point) * weight_seek_;
	}

	if (On(flee))
	{
		v_steering_force_ += Flee(pick_point) * weight_flee_;
	}

	if (On(arrive))
	{
		v_steering_force_ += Arrive(pick_point, deceleration_) * weight_arrive_;
	}

	if (On(pursuit))
	{
		assert(p_target_agent1_ && "pursuit target not assigned");

		v_steering_force_ += Pursuit(p_target_agent1_) * weight_pursuit_;
	}

	if (On(offset_pursuit))
	{
		assert(p_target_agent1_ && "pursuit target not assigned");
		assert(offset_.IsZero() && "No offset assigned");

		v_steering_force_ += Pursuit(p_target_agent1_) * weight_pursuit_;
	}

	if (On(evade)) {
		assert(p_target_agent1_ && "Evade target not assigned");

		v_steering_force_ += Evade(p_target_agent1_) * weight_evade_;
	}

	if (On(wander)){
		v_steering_force_ += Wander() * weight_wander_;
	}

	if (On(follow_path))
	{
		v_steering_force_ += FollowPath() * weight_follow_path_;
	}

	v_steering_force_.Truncate(p_vehicle_->physics_->MaxForce());

	return v_steering_force_;
}

CVector2D CSteeringBehavior::CalcuclatePrioritized() {
	return CVector2D(0.0f, 0.0f);
}

CVector2D CSteeringBehavior::CalculateDithered() {
	return CVector2D(0.0f, 0.0f);
}
