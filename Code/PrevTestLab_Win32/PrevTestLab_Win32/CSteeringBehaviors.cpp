#include<algorithm>
#include "CSteeringBehaviors.h"
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
	deceleration_(normal),
	wander_distance_(WANDER_DIST),
	wander_jitter_(WANDER_JITTER_PER_SEC),
	wander_radius_(WANDER_RAD),
	p_target_agent1_(nullptr),
	p_target_agent2_(nullptr)
{
	float theta = RandFloat() * TwoPi;
	wander_target_ = CVector2D(wander_radius_ * cos(theta), wander_radius_ * sin(theta));

	// TODO : Path = new Path();
	// TODO : Path->LoopOn();
	
}

CVector2D CSteeringBehavior::Calculate() {
	v_steering_force_.Zero();

	// TODO : 공간 분할 설정
	// TODO : 조종 행동 유형 변경
	v_steering_force_ = CalculateWeightedSum();

	return v_steering_force_;
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

	// TODO : LocalPosOfClosestObstacle...2018/07/29
	
	return CVector2D(0.0f, 0.0f);
}

//---------------------- CalculateWeightedSum ----------------------------
//
// 이것은 단순히 모든 '활성화된 동작들 * 가중치'를 합하고 
// 결과를 반환하기 전에 최대로 이용할 수 있는 조종힘으로 수치를 자릅니다.
//------------------------------------------------------------------------
CVector2D CSteeringBehavior::CalculateWeightedSum() {
	if (On(wall_avoidance)) {
		//TODO : wall_aboidance에 대한 행동 정의
	}

	if (On(obstacle_avoidance)) {
		//TODO : obstacle_avoidance에 대한 행동 정의
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

	v_steering_force_.Truncate(p_vehicle_->physics_->MaxForce());

	return v_steering_force_;
}

CVector2D CSteeringBehavior::CalcuclatePrioritized() {
	return CVector2D(0.0f, 0.0f);
}

CVector2D CSteeringBehavior::CalculateDithered() {
	return CVector2D(0.0f, 0.0f);
}
