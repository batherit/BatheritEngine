#include<algorithm>
#include<vector>
#include "CSteeringBehaviors.h"
#include "CObjectFuncTemplates.h"
#include "CUtil.h"
#include "CGameTimer.h"
#include "CGameWorld.h"
#include "CGameObject.h"
#include "CVehicle.h"
using namespace std;

CSteeringBehavior::CSteeringBehavior(CVehicle* agent) :
	p_vehicle_(agent),
	behavior_flags_(0),
	weight_separation_(VehiclePrm.separation_weight_),
	weight_cohesion_(VehiclePrm.cohesion_weight_),
	weight_alignment_(VehiclePrm.alignment_weight_),
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
	weight_interpose_(VehiclePrm.interpose_weight_),
	weight_hide_(VehiclePrm.hide_weight_),
	view_distance_(VehiclePrm.view_distance_),
	wall_detection_feeler_length_(VehiclePrm.wall_detection_feeler_length_),
	feelers_(3),
	deceleration_(normal),
	wander_distance_(WANDER_DIST),
	wander_jitter_(WANDER_JITTER_PER_SEC),
	wander_radius_(WANDER_RAD),
	waypoint_seek_dist_sq_(WAY_POINT_SEEK_DIST * WAY_POINT_SEEK_DIST),
	p_target_agent1_(nullptr),
	p_target_agent2_(nullptr),
	is_cell_space_on_(false),
	summing_method_(PRIORITIZED)
{
	float theta = RandFloat() * TwoPi;
	wander_target_ = CVector2D(wander_radius_ * cos(theta), wander_radius_ * sin(theta));

	p_path_ = new CPath2D();
	p_path_->LoopOn();
}

CSteeringBehavior::~CSteeringBehavior() { delete p_path_; }

CVector2D CSteeringBehavior::Calculate() {
	// 조종력을 0벡터로 세팅한다.
	v_steering_force_.Zero();

	// 공간 분할이 켜졌다면, 해당 비히클의 이웃을 계산하게 된다.
	// 그렇지 않다면 표준적인 태깅 시스템을 사용한다.
	if (!IsSpacePartitioningOn())
	{
		// 다음의 세 행동 중 하나라도 켜져있다면 이웃을 태그한다.
		if (On(separation) || On(allignment) || On(cohesion))
		{
			p_vehicle_->GameWorld()->TagAgentsWithinViewRange(p_vehicle_, view_distance_);
		}
	}
	else
	{
		// 다음 세 그룹 행동 중 하나라도 켜져있다면, 셀 공간에서 이웃들을 계산한다.
		if (On(separation) || On(allignment) || On(cohesion))
		{
			p_vehicle_->GameWorld()->CellSpace()->CalculateNeighbors(p_vehicle_->transform_.pos_, view_distance_);
		}
	}

	switch (summing_method_) {
	case WEIGHTED_AVERAGE:
		v_steering_force_ = CalculateWeightedSum();
		break;
	case PRIORITIZED:
		v_steering_force_ = CalculatePrioritized();
		break;
	case DITHERED:
		v_steering_force_ = CalculateDithered();
		break;
	default:
		v_steering_force_ = CVector2D(0.0f, 0.0f);
	} // 스위치문 끝

	return v_steering_force_;
}

//--------------------- AccumulateForce ----------------------------------
//
//	이 함수는 얼마나 많은 최대 조종력을 비히클이 적용하기 위해
//	남길 것인지를 계산한다. 그리고 이것은 더해질 힘의 양을 적용한다.
//------------------------------------------------------------------------
bool CSteeringBehavior::AccumulateForce(CVector2D &RunningTot,
	CVector2D ForceToAdd)
{
	// 얼마나 많은 조종력을 비히클이 '지금까지' 사용해왔는지를 계산한다.
	float magnitude_sofar = RunningTot.Length();

	// 얼마나 많은 조종력을 비히클이 사용할 것인지를 계산한다.
	float magnitude_remaining = p_vehicle_->physics_->MaxForce() - magnitude_sofar;

	// 사용될 남은 힘이 더 이상 없다면 false를 반환한다.
	if (magnitude_remaining <= 0.0f) return false;

	// 추가되길 원하는 힘의 양을 계산한다.
	float magnitude_to_add = ForceToAdd.Length();

	// ForceToAdd와 계산된 합의 양이 해당 비히클이 이용할 수 있는 최대 힘을
	// 초과하지 않는다면 합산하여 저장한다. 그렇지 않다면 ForceToAdd 벡터를
	// 최대 힘을 넘지 않는 선에서 가능한 더한다.
	if (magnitude_to_add < magnitude_remaining)
	{
		RunningTot += ForceToAdd;
	}

	else
	{
		// 이것을 조종력에 더한다.
		RunningTot += (Vec2DNormalize(ForceToAdd) * magnitude_remaining);
	}

	return true;
}

void CSteeringBehavior::CreateFeelers() {
	// 앞쪽 감지기
	feelers_[0] = p_vehicle_->transform_.pos_ + wall_detection_feeler_length_ * p_vehicle_->transform_.look_;

	// 왼쪽 감지기
	CVector2D temp = p_vehicle_->transform_.look_;
	Vec2DRotateAroundOrigin(temp, HalfPi * 3.5f);	// y축이 반전된 상태란 것을 고려
	feelers_[1] = p_vehicle_->transform_.pos_ + wall_detection_feeler_length_ / 2.0f * temp;

	// 오른쪽 감지기
	temp = p_vehicle_->transform_.look_;
	Vec2DRotateAroundOrigin(temp, HalfPi * 0.5f);	// y축이 반전된 상태란 것을 고려
	feelers_[2] = p_vehicle_->transform_.pos_ + wall_detection_feeler_length_ / 2.0f * temp;
}

//this behavior moves the agent towards a target position
CVector2D CSteeringBehavior::Seek(CVector2D target_pos) {
	CVector2D desired_velocity = Vec2DNormalize(target_pos - p_vehicle_->transform_.pos_)
		* p_vehicle_->physics_->MaxSpeed();

	return (desired_velocity - p_vehicle_->physics_->Velocity());
}

//---------------------------- 분할 --------------------------------
//
// 이것은 다른 이웃들로부터 반발하는 힘들 계산한다.
//------------------------------------------------------------------------
CVector2D CSteeringBehavior::Separation(const vector<CGameObject*> &neighbors)
{
	CVector2D steering_force;

	for (unsigned int a = 0; a<neighbors.size(); ++a)
	{
		// 해당 에이전트가 계산에 포함되지 않으며, 조사하는 에이전트가 충분히 가까이
		// 있음을 확인한다. *** 또한 이것이 회피 대상을 포함하지 않는 것을 확인한다.
		if ((neighbors[a] != p_vehicle_) && neighbors[a]->IsTagged() &&
			(neighbors[a] != p_target_agent1_))
		{
			CVector2D ToAgent = p_vehicle_->transform_.pos_ - neighbors[a]->transform_.pos_;

			// 이웃으로부터 떨어진 에이전트 거리에 역비례하여 힘을 조정한다.
			steering_force += Vec2DNormalize(ToAgent) / ToAgent.Length();
		}
	}

	return steering_force;
}


//---------------------------- 정렬 ---------------------------------
//
//  이웃들에 향하는 에이전트들을 정렬하도록하는 
//  힘을 반환한다.
//------------------------------------------------------------------------
CVector2D CSteeringBehavior::Alignment(const vector<CGameObject*>& neighbors)
{
	// 이웃들의 평균 헤딩 벡터를 기록하는데 사용된다.
	CVector2D average_heading;

	// 이웃에 있는 비히클의 수를 세는데 쓰인다.
	int    neighbor_count = 0;

	// 모든 태그된 비히클들을 순회하고 그들의 헤딩 벡터들을 합한다. 
	for (unsigned int a = 0; a<neighbors.size(); ++a)
	{
		// 해당 에이전트가 계산에 포함되지 않으며, 조사되는 에이전트가 충분히 가까이
		// 있음을 확인한다. *** 또한 이것은 회피 타겟을 포함하지 않음을 확인한다.***

		if ((neighbors[a] != p_vehicle_) && neighbors[a]->IsTagged() &&
			(neighbors[a] != p_target_agent1_))
		{
			average_heading += neighbors[a]->transform_.look_;

			++neighbor_count;
		}
	}

	// 이웃이 하나 이상의 비히클들을 포함한다면, 그들의 헤딩 벡터들을
	// 평균한다.
	if (neighbor_count > 0)
	{
		average_heading /= (float)neighbor_count;

		average_heading -= p_vehicle_->transform_.look_;
	}

	// 비히클을 이웃들의 평균 헤딩 벡터가 가리키는 곳으로 향하게 된다.
	return average_heading;
}

//-------------------------------- 결합 ------------------------------
//
//  즉각 지역에서 '군중의 중심'을 향하도록 에이전트를 움직이도록하는
//  조종힘을 반환한다.
//------------------------------------------------------------------------
CVector2D CSteeringBehavior::Cohesion(const vector<CGameObject*> &neighbors)
{
	// 첫째로 군중의 중심을 찾는다.
	CVector2D center_of_mass, steering_force;

	int neighbor_count = 0;

	// 이웃들을 순회하고 모든 위치 벡터들을 더한다.
	for (unsigned int a = 0; a<neighbors.size(); ++a)
	{
		// 해당 에이전트가 계산에 포함되지 않고 조사되는 에이전트가 충분히 가까움을 
		// 확인한다. ***또한 이것이 회피 대상을 포함하지 않음을 확인한다. ***
		if ((neighbors[a] != p_vehicle_) && neighbors[a]->IsTagged() &&
			(neighbors[a] != p_target_agent1_))
		{
			center_of_mass += neighbors[a]->transform_.pos_;

			++neighbor_count;
		}
	}

	if (neighbor_count > 0)
	{
		// 질량 중심은 위치 좌표 합에 대한 평균이다.
		center_of_mass /= (float)neighbor_count;

		// 현재 그 좌표를 향해 '찾기'행동을 수행한다.
		steering_force = Seek(center_of_mass);
	}

	// 결합의 정도는 보통 분리 또는 정렬에 대한 경우보다 크기 때문에 
	// 보통 이것을 정규화한다.(?)
	return Vec2DNormalize(steering_force);
}


/* 주의: 다음 세 가지 행동은 위와 같다. 다만 이웃을 찾기 위해 셀 공간 분할을 
사용한다는 점에서 다르다.*/


//---------------------------- Separation --------------------------------
//
// 다른 이웃들에 반발하는 힘을 계산한다.
//
//  공간 분할을 사용한다.
//------------------------------------------------------------------------
CVector2D CSteeringBehavior::SeparationPlus(const vector<CGameObject*> &neighbors)
{
	CVector2D steering_force;

	// 이웃들을 순회하며, 모든 위치 벡터를 합한다.
	for (CGameObject* pV = p_vehicle_->GameWorld()->CellSpace()->begin();
		!p_vehicle_->GameWorld()->CellSpace()->end();
		pV = p_vehicle_->GameWorld()->CellSpace()->next())
	{
		// 해당 에이전트가 계산에 포함되지 않으며, 조사되는 에이전트가
		// 충분히 가까움을 확인한다.
		if (pV != p_vehicle_)
		{
			CVector2D to_agent = p_vehicle_->transform_.pos_ - pV->transform_.pos_;

			// 에이전트가 이웃에 떨어진 정도에 역비례하여 힘을 조정한다.
			steering_force += Vec2DNormalize(to_agent) / to_agent.Length();
		}

	}

	return steering_force;
}
//---------------------------- Alignment ---------------------------------
//
//  이웃들의 헤딩 벡터로 에이전트 헤딩 벡터를 
//  정렬하려는 힘을 반환한다.
//
//  공간 분할을 사용한다.
//------------------------------------------------------------------------
CVector2D CSteeringBehavior::AlignmentPlus(const vector<CGameObject*> &neighbors)
{
	// 이것은 이웃들의 평균 헤딩 벡터를 기록한다.
	CVector2D average_heading;

	// 이것은 이웃한 비히클들의 수를 센다.
	float    neighbor_count = 0.0f;

	// 이웃들을 순회하며 모든 위치 좌표들을 합한다.
	for (CGameObject* pV = p_vehicle_->GameWorld()->CellSpace()->begin();
		!p_vehicle_->GameWorld()->CellSpace()->end();
		pV = p_vehicle_->GameWorld()->CellSpace()->next())
	{
		// 해당 에이전트가 계산에 포함되지 않으며 조사되는 에이전트가
		// 충분히 가까이 있음을 확인한다.
		if (pV != p_vehicle_)
		{
			average_heading += pV->transform_.look_;

			++neighbor_count;
		}

	}

	// 이웃이 하나 이상이라면, 그들의 헤딩 벡터를 
	// 평균한다.
	if (neighbor_count > 0.0f)
	{
		average_heading /= neighbor_count;

		average_heading -= p_vehicle_->transform_.look_;
	}

	return average_heading;
}


//-------------------------------- Cohesion ------------------------------
//
//  즉시 지역에서 군중 중심에 향하도록 에이전트를 움직이려는 조종력을
//  반환한다.
//
//  공간 분할을 사용한다.
//------------------------------------------------------------------------
CVector2D CSteeringBehavior::CohesionPlus(const vector<CGameObject*> &neighbors)
{
	// 첫째로 군중 중심을 찾는다.
	CVector2D center_of_mass, steering_force;

	int neighbor_count = 0;

	// 이웃들을 순회하며 모든 위치 벡터를 합한다.
	for (CGameObject* pV = p_vehicle_->GameWorld()->CellSpace()->begin();
		!p_vehicle_->GameWorld()->CellSpace()->end();
		pV = p_vehicle_->GameWorld()->CellSpace()->next())
	{
		// 해당 에이전트가 계산에 포함되지 않으며, 조사되는 에이전트가 충분히
		// 가까이 있음을 확인한다.
		if (pV != p_vehicle_)
		{
			center_of_mass += pV->transform_.pos_;

			++neighbor_count;
		}
	}

	if (neighbor_count > 0)
	{
		// 군중 중심은 위치 벡터 합에 대한 평균이다.
		center_of_mass /= (float)neighbor_count;

		// 그곳으로 향한 '찾기'행동을 수행한다.
		//now seek towards that position
		steering_force = Seek(center_of_mass);
	}

	// 결합의 정도는 보통 분할 또는 정렬보다 크기 때문에
	// 보통 이것을 정규화한다. (?)
	return Vec2DNormalize(steering_force);
}

// 해당 해동은 타겟 위치로부터 에이전트를 떨어뜨리는 
// 벡터를 반환한다.
CVector2D CSteeringBehavior::Flee(CVector2D target_pos) {
	
	// 패닉 상태(제곱 거리 판별)에 있을 때에만 '달아나기(Flee)'가 유효하도록 한다.
	// 타겟과의 거리가 100 이하가 아닐 경우 Flee를 작동시키지 않는다.
	const float panic_distance_sq = 100.0f * 100.0f;		
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
		// (to_target / dist => 정규화된 to_target)
		CVector2D desired_velocity = to_target * speed / dist;

		return (desired_velocity - p_vehicle_->physics_->Velocity());
	}

	return CVector2D(0, 0);
}

// [0~1]값을 반환 : 대면 - 0, 반대 - 1
float CSteeringBehavior::TurnAroundTime(const CVehicle* agent, CVector2D target_pos) const {
	// 목표까지의 정규화된 벡터를 결정한다.
	CVector2D to_target = Vec2DNormalize(target_pos - agent->transform_.pos_);

	float dot = agent->transform_.look_.Dot(to_target);

	const float coefficient = 0.5f;

	return (dot - 1.0f) * -coefficient;
}

// '추격하기(Pursuit)'은 에이전트의 미래 위치를 예측하여 그 지점으로 이동한다.
// 목표물을 가로챌 때 유용하다.
CVector2D CSteeringBehavior::Pursuit(const CVehicle* evader) {
	// 만약 도피자가 앞에 있고 에이전트를 대면하고 있다면, 도피자의
	// 현재 위치만을 찾을 수 있다.
	CVector2D to_evader = evader->transform_.pos_ - p_vehicle_->transform_.pos_;

	float relative_look = p_vehicle_->transform_.look_.Dot(evader->transform_.look_);

	if ((to_evader.Dot(p_vehicle_->transform_.look_) > 0.0f) &&
		(relative_look < -0.95f))  //acos(0.95)=18 degs
	{
		return Seek(evader->transform_.pos_);
	}

	// 바로 앞에 있지 않다고 간주되면, 도피자가 어디 있을 것인지를 예측한다.

	// 예측 시간은 도피자와 추격자 사이의 거리에 비례하고,
	// 이 에이전트들의 속도의 합에는 반비례한다.
	float look_ahead_time = to_evader.Length() /
		(p_vehicle_->physics_->MaxSpeed() + evader->physics_->Speed());

	look_ahead_time += TurnAroundTime(p_vehicle_, evader->transform_.pos_);
	// 이제 도피자가 있을 것으로 예측된 미래 위치로 찾아간다.
	return Seek(evader->transform_.pos_ + evader->physics_->Velocity() * look_ahead_time);
}

//------------------------- Offset Pursuit -------------------------------
//
//  리더 비히클로부터 특정된 오프셋으로 비히클을 두려는 조종힘을 
//  만들어낸다. 오프셋은 선도자의 로컬 공간에 정의된다.
//------------------------------------------------------------------------
CVector2D CSteeringBehavior::OffsetPursuit(const CVehicle* leader, const CVector2D offset) {
	// 월드 공간에서의 오프셋 위치를 구한다.
	CVector2D world_offset_pos = PointToWorldSpace(offset,
		leader->transform_.look_,
		leader->transform_.right_,
		leader->transform_.pos_);

	CVector2D to_offset = world_offset_pos - p_vehicle_->transform_.pos_;

	// 예측되는 시간은 리더와 추적자 사이의
	// 거리에 비례하며, 두 에이전트 속도합에 반비례한다.
	float lookahead_time = to_offset.Length() /
		(p_vehicle_->physics_->MaxSpeed() + leader->physics_->Speed());

	// 오프셋의 예상되는 미래 위치로 도착하게끔 한다.
	return Arrive(world_offset_pos + leader->physics_->Velocity() * lookahead_time, fast);
}

// 예측된 위치로부터 회피하는 조종힘을 반환한다.
CVector2D CSteeringBehavior::Evade(const CVehicle* pursuer) {
	/* 이번에 직면하는지를 체크하는 코드를 포함하는 것은 불필요하다. */

	CVector2D to_pursuer = pursuer->transform_.pos_ - p_vehicle_->transform_.pos_;

	// 단지 위협 거리 안에서만 해당 함수를 동작하게 하려면
	// 다음의 두 라인을 추가한다.
	const float threat_range = 100.0f;
	if (to_pursuer.LengthSq() > threat_range * threat_range) return CVector2D();

	//the lookahead time is propotional to the distance between the pursuer
	//and the pursuer; and is inversely proportional to the sum of the
	//agents' velocities
	float LookAheadTime = to_pursuer.Length() /
		(p_vehicle_->physics_->MaxSpeed() + pursuer->physics_->Speed());

	//now flee away from predicted future position of the pursuer
	return Flee(pursuer->transform_.pos_ + pursuer->physics_->Velocity() * LookAheadTime);
}

// 해당 행동은 에이전트가 무작위로 배회하도록 한다.
CVector2D CSteeringBehavior::Wander() {
	/*해당 행동은 갱신율에 의존하기에, 아래의 라인은 시간 독립적인 프레임율을 사용할 때
	포함된다.*/
	float jitter_this_time_slice = wander_jitter_ * WorldTimer->GetElapsedTimePerFrame();

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
	// 감지 상자의 길이는 에이전트의 속도에 비례한다. (최대 min_detecction_box_length의 2배 길이)
	detection_box_length_ = VehiclePrm.min_detection_box_length_ +
		(p_vehicle_->physics_->Speed() / p_vehicle_->physics_->MaxSpeed()) *
		VehiclePrm.min_detection_box_length_;

	// 감지 상자 범위 내의 장애물들을 처리하기 위해 표시해둔다.
	// 감지 박스의 길이를 반지름으로 하는 원 내부에 장애물이 들어올 경우,
	// 그 장애물의 태그를 건다.
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
					p_vehicle_->Mesh()->GetBoundingRad();	// 비히클의 경계 반경

				if (fabs(local_pos.y_) < expanded_radius)
				{
					// 이제 선 / 원 교차 테스트를 수행한다. 원의 중심은(cX, cY)로 표시된다.
					// 교점은 y = 0에 대한 공식 x = cX +/-sqrt(r ^ 2 - cY ^ 2)로 표시된다.
					// (원의 방정식을 참고.)
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
	// feelers_ 벡터에 감지기 정보가 들어간다.
	CreateFeelers();

	float dist_to_this_IP = 0.0f;
	float dist_to_closest_IP = MaxFloat;

	// 이것은 벽 벡터에 대한 한 인덱스를 저장할 것이다.
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

//--------------------------- Interpose ----------------------------------
//
//  두 개의 에이전트가 주어진다면, 이 메서드는 그들 사이에 비히클을
//  위치시키려는 힘을 반환한다.
//------------------------------------------------------------------------
CVector2D CSteeringBehavior::Interpose(const CVehicle* vehicleA, const CVehicle* vehicleB) {
	// 첫째로, 우리는 두 에이전트가 미래의 시간 T에 대하여 어디에 있을지에 대해 파악할 
	// 필요가 있다. 이것은 최대 속도에서 현재 시간에 대하여 중앙 지점으로 도달하기까지
	// 걸리는 시간을 결정함으로써 근사할 수 있다.
	CVector2D mid_point = (vehicleA->transform_.pos_ + vehicleB->transform_.pos_) / 2.0f;

	float time_to_reach_mid_point = Vec2DDistance(p_vehicle_->transform_.pos_, mid_point) /
		p_vehicle_->physics_->MaxSpeed();

	// 이를 통해 시간 T를 가지게 되었다. 우리는 에이전트와 A와 B가 계속 직선 궤적에
	// 있을 것이라 가정하고 그들의 미래 위치를 추론한다.
	CVector2D posA = vehicleA->transform_.pos_ + vehicleA->physics_->Velocity() * time_to_reach_mid_point;
	CVector2D posB = vehicleB->transform_.pos_ + vehicleB->physics_->Velocity() * time_to_reach_mid_point;

	// 예상된 지점들의 중앙값을 계산한다.
	mid_point = (posA + posB) / 2.0f;

	// 그곳에 도착하도록 조종한다.
	return Arrive(mid_point, fast);
}

//--------------------------- Hide ---------------------------------------
//
//	이 행동은 에이전트와 추적자 사이에 항상 장애물이 놓이도록 하는
//	조종힘을 반환한다.
//------------------------------------------------------------------------
CVector2D CSteeringBehavior::Hide(const CVehicle* hunter,
	const vector<CGameObject*>& obstacles)
{
	float dist_to_closest = MaxFloat;
	CVector2D best_hiding_spot;

	std::vector<CGameObject*>::const_iterator curOb = obstacles.begin();
	std::vector<CGameObject*>::const_iterator closest;

	while (curOb != obstacles.end())
	{
		// 해당 장애물에 대하여 숨을 곳의 위치를 계산한다.
		CVector2D hiding_spot = GetHidingPosition((*curOb)->transform_.pos_,
			(*curOb)->Mesh()->GetBoundingRad(),
			hunter->transform_.pos_);

		// 에이전트에서 가장 가까운 숨을 장소를 찾기 위해 거리 제곱 공간에서
		// 작동한다.
		float dist = Vec2DDistanceSq(hiding_spot, p_vehicle_->transform_.pos_);

		if (dist < dist_to_closest)
		{
			dist_to_closest = dist;

			best_hiding_spot = hiding_spot;

			closest = curOb;
		}

		++curOb;

	}// while문 끝

	// 적절한 장애물들을 찾지 못했다면, 헌터를 피하도록 한다.
	if (dist_to_closest == MaxFloat)
	{
		return Evade(hunter);
	}

	// 그렇지 않다면 숨을 장소로 '도착하기'를 사용한다.
	return Arrive(best_hiding_spot, fast);
}

//------------------------- GetHidingPosition ----------------------------
//
//  헌터의 위치, 비히클의 위치, 그리고 장애물의 둘레가 주어진다면,
//  해당 메서드는 바운딩 반지름만큼 떨어진 그리고 헌터에 정반대편에 있는 경계선의 위치를
//  계산한다.
//------------------------------------------------------------------------
CVector2D CSteeringBehavior::GetHidingPosition(const CVector2D& posOb,
	const float     radiusOb,
	const CVector2D& posHunter)
{
	// 에이전트가 선택된 장애물의 바운딩 반지름에 얼마만큼 떨어져있는지를
	// 계산한다.
	const float distance_from_boundary = 30.0f;
	float dist_away = radiusOb + distance_from_boundary;

	// 헌터에서 오브젝트로 향하는 헤딩 벡터를 계산한다.
	CVector2D ToOb = Vec2DNormalize(posOb - posHunter);

	// 크기를 조정하고 장애물 위치에 추가하여 숨는 지점을
	// 찾아낸다.
	return (ToOb * dist_away) + posOb;
}

//---------------------- CalculatePrioritized ----------------------------
//
//	해당 메서드는 각 활성화된 행동을 우선순위에 따라 호출하며, 최대힘에
//	도달하기 전까지 각 행동에 대한 조종힘들을 누적한다. 최대힘까지 누적된
//	조종력을 반환한다.
//------------------------------------------------------------------------
CVector2D CSteeringBehavior::CalculatePrioritized()
{
	CVector2D force;

	if (On(wall_avoidance))
	{
		force = WallAvoidance(p_vehicle_->GameWorld()->Walls()) * weight_wall_avoidance_;

		if (!AccumulateForce(v_steering_force_, force)) return v_steering_force_;
	}

	if (On(obstacle_avoidance))
	{
		force = ObstacleAvoidance(p_vehicle_->GameWorld()->Obstacles()) * weight_obstacle_avoidance_;

		if (!AccumulateForce(v_steering_force_, force)) return v_steering_force_;
	}

	if (On(evade))
	{
		assert(p_target_agent1_ && "Evade target not assigned");

		force = Evade(p_target_agent1_) * weight_evade_;

		if (!AccumulateForce(v_steering_force_, force)) return v_steering_force_;
	}


	if (On(flee))
	{
		force = Flee(p_vehicle_->GameWorld()->PickPoint()) * weight_flee_;

		if (!AccumulateForce(v_steering_force_, force)) return v_steering_force_;
	}


	// 다음의 세 가지는 무리 행동을 위해 결합된다.
	// (wander는 이 결합에 추가되봄직하다.)
	if (!IsSpacePartitioningOn())
	{
		if (On(separation))
		{
			force = Separation(p_vehicle_->GameWorld()->Agents()) * weight_separation_;

			if (!AccumulateForce(v_steering_force_, force)) return v_steering_force_;
		}

		if (On(allignment))
		{
			force = Alignment(p_vehicle_->GameWorld()->Agents()) * weight_alignment_;

			if (!AccumulateForce(v_steering_force_, force)) return v_steering_force_;
		}

		if (On(cohesion))
		{
			force = Cohesion(p_vehicle_->GameWorld()->Agents()) * weight_cohesion_;

			if (!AccumulateForce(v_steering_force_, force)) return v_steering_force_;
		}
	}

	else
	{

		if (On(separation))
		{
			force = SeparationPlus(p_vehicle_->GameWorld()->Agents()) * weight_separation_;

			if (!AccumulateForce(v_steering_force_, force)) return v_steering_force_;
		}

		if (On(allignment))
		{
			force = AlignmentPlus(p_vehicle_->GameWorld()->Agents()) * weight_alignment_;

			if (!AccumulateForce(v_steering_force_, force)) return v_steering_force_;
		}

		if (On(cohesion))
		{
			force = CohesionPlus(p_vehicle_->GameWorld()->Agents()) * weight_cohesion_;

			if (!AccumulateForce(v_steering_force_, force)) return v_steering_force_;
		}
	}

	if (On(seek))
	{
		force = Seek(p_vehicle_->GameWorld()->PickPoint()) * weight_seek_;

		if (!AccumulateForce(v_steering_force_, force)) return v_steering_force_;
	}


	if (On(arrive))
	{
		force = Arrive(p_vehicle_->GameWorld()->PickPoint(), deceleration_) * weight_arrive_;

		if (!AccumulateForce(v_steering_force_, force)) return v_steering_force_;
	}

	if (On(wander))
	{
		force = Wander() * weight_wander_;

		if (!AccumulateForce(v_steering_force_, force)) return v_steering_force_;
	}

	if (On(pursuit))
	{
		assert(p_target_agent1_ && "pursuit target not assigned");

		force = Pursuit(p_target_agent1_) * weight_pursuit_;

		if (!AccumulateForce(v_steering_force_, force)) return v_steering_force_;
	}

	if (On(offset_pursuit))
	{
		assert(p_target_agent1_ && "pursuit target not assigned");
		assert(!offset_.IsZero() && "No offset assigned");

		force = OffsetPursuit(p_target_agent1_, offset_);

		if (!AccumulateForce(v_steering_force_, force)) return v_steering_force_;
	}

	if (On(interpose))
	{
		assert(p_target_agent1_ && p_target_agent2_ && "Interpose agents not assigned");

		force = Interpose(p_target_agent1_, p_target_agent2_) * weight_interpose_;

		if (!AccumulateForce(v_steering_force_, force)) return v_steering_force_;
	}

	if (On(hide))
	{
		assert(p_target_agent1_ && "Hide target not assigned");

		force = Hide(p_target_agent1_, p_vehicle_->GameWorld()->Obstacles()) * weight_hide_;

		if (!AccumulateForce(v_steering_force_, force)) return v_steering_force_;
	}


	if (On(follow_path))
	{
		force = FollowPath() * weight_follow_path_;

		if (!AccumulateForce(v_steering_force_, force)) return v_steering_force_;
	}

	return v_steering_force_;
}

//---------------------- CalculateWeightedSum ----------------------------
//
// 이것은 단순히 모든 '활성화된 동작들 * 가중치'를 합하고 
// 결과를 반환하기 전에 최대로 이용할 수 있는 조종힘으로 수치를 자릅니다.
//------------------------------------------------------------------------
CVector2D CSteeringBehavior::CalculateWeightedSum() {
	// 다음 세 가지 행동은 집단 행동으로 결합될 수 있다. 
	// (wander는 이런 결합에 추가되봄직 하다.)
	if (!IsSpacePartitioningOn())
	{
		if (On(separation))
		{
			
			v_steering_force_ += Separation(p_vehicle_->GameWorld()->Agents()) *weight_separation_;
		}

		if (On(allignment))
		{
			v_steering_force_ += Alignment(p_vehicle_->GameWorld()->Agents()) * weight_alignment_;
		}

		if (On(cohesion))
		{
			v_steering_force_ += Cohesion(p_vehicle_->GameWorld()->Agents()) * weight_cohesion_;
		}
	}
	else
	{
		if (On(separation))
		{
			v_steering_force_ += SeparationPlus(p_vehicle_->GameWorld()->Agents()) * weight_separation_;
		}

		if (On(allignment))
		{
			v_steering_force_ += AlignmentPlus(p_vehicle_->GameWorld()->Agents()) * weight_alignment_;
		}

		if (On(cohesion))
		{
			v_steering_force_ += CohesionPlus(p_vehicle_->GameWorld()->Agents()) * weight_cohesion_;
		}
	}
	
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

	if (On(interpose))
	{
		assert(p_target_agent1_ && p_target_agent2_ && "Interpose agents not assigned");

		v_steering_force_ += Interpose(p_target_agent1_, p_target_agent2_) * weight_interpose_;
	}

	if (On(hide))
	{
		assert(p_target_agent1_ && "Hide target not assigned");

		v_steering_force_ += Hide(p_target_agent1_, p_vehicle_->GameWorld()->Obstacles()) * weight_hide_;
	}

	v_steering_force_.Truncate(p_vehicle_->physics_->MaxForce());

	return v_steering_force_;
}

//---------------------- CalculateDithered ----------------------------
//
//	이 메서드는 각 행동에서 계산된 확률을 할당함으로써 활성화된 행동들을
//	합한다. 그런 다음 이 시뮬레이션 단계를 계산해야만 하는지를 알기 위해
//	우선 순위를 테스트한다. 그런 경우, 이것은 해당 행동으로 인한 
//	조종력을 계산한다. 그것이 영벡터보다 크다면 힘을 반환하고 영벡터이거나
//	해당 행동이 스킵될 경우, 계속해서 마찬가지로 다음 순위의 행동을 진행한다.
//
//	주의 : 모든 행동이 해당 메서드에서 실행되는 것은 아니다. 단지 몇 가지일 뿐이며
//	일반적인 원리를 터득하게 될 것이다.
//------------------------------------------------------------------------
CVector2D CSteeringBehavior::CalculateDithered()
{
	// 조종력을 리셋한다.
	v_steering_force_.Zero();

	if (On(wall_avoidance) && RandFloat() < VehiclePrm.pr_wall_avoidance_)
	{
		v_steering_force_ = WallAvoidance(p_vehicle_->GameWorld()->Walls()) *
			weight_wall_avoidance_ / VehiclePrm.pr_wall_avoidance_;

		if (!v_steering_force_.IsZero())
		{
			v_steering_force_.Truncate(p_vehicle_->physics_->MaxForce());

			return v_steering_force_;
		}
	}

	if (On(obstacle_avoidance) && RandFloat() < VehiclePrm.pr_obstacle_avoidance_)
	{
		v_steering_force_ += ObstacleAvoidance(p_vehicle_->GameWorld()->Obstacles()) *
			weight_obstacle_avoidance_ / VehiclePrm.pr_obstacle_avoidance_;

		if (!v_steering_force_.IsZero())
		{
			v_steering_force_.Truncate(p_vehicle_->physics_->MaxForce());

			return v_steering_force_;
		}
	}

	if (!IsSpacePartitioningOn())
	{
		if (On(separation) && RandFloat() < VehiclePrm.pr_separation_)
		{
			v_steering_force_ += Separation(p_vehicle_->GameWorld()->Agents()) *
				weight_separation_ / VehiclePrm.pr_separation_;

			if (!v_steering_force_.IsZero())
			{
				v_steering_force_.Truncate(p_vehicle_->physics_->MaxForce());

				return v_steering_force_;
			}
		}
	}

	else
	{
		if (On(separation) && RandFloat() < VehiclePrm.pr_separation_)
		{
			v_steering_force_ += SeparationPlus(p_vehicle_->GameWorld()->Agents()) *
				weight_separation_ / VehiclePrm.pr_separation_;

			if (!v_steering_force_.IsZero())
			{
				v_steering_force_.Truncate(p_vehicle_->physics_->MaxForce());

				return v_steering_force_;
			}
		}
	}


	if (On(flee) && RandFloat() < VehiclePrm.pr_flee_)
	{
		v_steering_force_ += Flee(p_vehicle_->GameWorld()->PickPoint()) * weight_flee_ / VehiclePrm.pr_flee_;

		if (!v_steering_force_.IsZero())
		{
			v_steering_force_.Truncate(p_vehicle_->physics_->MaxForce());

			return v_steering_force_;
		}
	}

	if (On(evade) && RandFloat() < VehiclePrm.pr_evade_)
	{
		assert(p_target_agent1_ && "Evade target not assigned");

		v_steering_force_ += Evade(p_target_agent1_) * weight_evade_ / VehiclePrm.pr_evade_;

		if (!v_steering_force_.IsZero())
		{
			v_steering_force_.Truncate(p_vehicle_->physics_->MaxForce());

			return v_steering_force_;
		}
	}


	if (!IsSpacePartitioningOn())
	{
		if (On(allignment) && RandFloat() < VehiclePrm.pr_alignment_)
		{
			v_steering_force_ += Alignment(p_vehicle_->GameWorld()->Agents()) *
				weight_alignment_ / VehiclePrm.pr_alignment_;

			if (!v_steering_force_.IsZero())
			{
				v_steering_force_.Truncate(p_vehicle_->physics_->MaxForce());

				return v_steering_force_;
			}
		}

		if (On(cohesion) && RandFloat() < VehiclePrm.pr_cohesion_)
		{
			v_steering_force_ += Cohesion(p_vehicle_->GameWorld()->Agents()) *
				weight_cohesion_ / VehiclePrm.pr_cohesion_;

			if (!v_steering_force_.IsZero())
			{
				v_steering_force_.Truncate(p_vehicle_->physics_->MaxForce());

				return v_steering_force_;
			}
		}
	}
	else
	{
		if (On(allignment) && RandFloat() < VehiclePrm.pr_alignment_)
		{
			v_steering_force_ += AlignmentPlus(p_vehicle_->GameWorld()->Agents()) *
				weight_alignment_ / VehiclePrm.pr_alignment_;

			if (!v_steering_force_.IsZero())
			{
				v_steering_force_.Truncate(p_vehicle_->physics_->MaxForce());

				return v_steering_force_;
			}
		}

		if (On(cohesion) && RandFloat() < VehiclePrm.pr_cohesion_)
		{
			v_steering_force_ += CohesionPlus(p_vehicle_->GameWorld()->Agents()) *
				weight_cohesion_ / VehiclePrm.pr_cohesion_;

			if (!v_steering_force_.IsZero())
			{
				v_steering_force_.Truncate(p_vehicle_->physics_->MaxForce());

				return v_steering_force_;
			}
		}
	}

	if (On(wander) && RandFloat() < VehiclePrm.pr_wander_)
	{
		v_steering_force_ += Wander() * weight_wander_ / VehiclePrm.pr_wander_;

		if (!v_steering_force_.IsZero())
		{
			v_steering_force_.Truncate(p_vehicle_->physics_->MaxForce());

			return v_steering_force_;
		}
	}

	if (On(seek) && RandFloat() < VehiclePrm.pr_seek_)
	{
		v_steering_force_ += Seek(p_vehicle_->GameWorld()->PickPoint()) * weight_seek_ / VehiclePrm.pr_seek_;

		if (!v_steering_force_.IsZero())
		{
			v_steering_force_.Truncate(p_vehicle_->physics_->MaxForce());

			return v_steering_force_;
		}
	}

	if (On(arrive) && RandFloat() < VehiclePrm.pr_arrive_)
	{
		v_steering_force_ += Arrive(p_vehicle_->GameWorld()->PickPoint(), deceleration_) *
			weight_arrive_ / VehiclePrm.pr_arrive_;

		if (!v_steering_force_.IsZero())
		{
			v_steering_force_.Truncate(p_vehicle_->physics_->MaxForce());

			return v_steering_force_;
		}
	}

	return v_steering_force_;
}
