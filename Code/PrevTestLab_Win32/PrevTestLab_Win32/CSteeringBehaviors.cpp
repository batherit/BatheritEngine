#include<algorithm>
#include"CUtil.h"
#include "CSteeringBehaviors.h"
#include "CVehicle.h"
using namespace std;

CSteeringBehavior::CSteeringBehavior(CVehicle* agent) :
	p_vehicle_(agent),
	behavior_flags_(0),
	weight_seek_(),
	weight_flee_(),
	weight_arrive_(),
	weight_pursuit_(),
	weight_offset_pursuit_(),
	weight_evade_(),
	weight_wander_(),
	deceleration_(normal),
	wander_distance_(),
	wander_jitter_(),
	wander_radius_(),
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

	return v_steering_force_ = v_steering_force_;
}

//this behavior moves the agent towards a target position
CVector2D CSteeringBehavior::Seek(CVector2D target_pos) {
	CVector2D desired_velocity = Vec2DNormalize(target_pos - p_vehicle_->p_owner_->transform_.pos_)
		* p_vehicle_->max_speed_;

	return (desired_velocity - p_vehicle_->v_velocity_);
}

//this behavior returns a vector that moves the agent away
//from a target position
CVector2D CSteeringBehavior::Flee(CVector2D target_pos) {
	
	// 패닉 상태(제곱 거리 판별)에 있을 때에만 '달아나기(Flee)'가 유효하도록 한다.
	const double panic_distance_sq = 100.0f * 100.0f;
	if (Vec2DDistanceSq(p_vehicle_->p_owner_->transform_.pos_, target_pos) > panic_distance_sq) {
		return CVector2D(0,0);
	}
	

	CVector2D desired_velocity = Vec2DNormalize(p_vehicle_->p_owner_->transform_.pos_ - target_pos)
		* p_vehicle_->max_speed_;

	return (desired_velocity - p_vehicle_->v_velocity_);
}

// 목표에 도달하기 위한 가감속이 있는 '찾기(Seek)'행동이라 보면 될 것이다.
CVector2D CSteeringBehavior::Arrive(CVector2D     TargetPos, Deceleration deceleration) {
	CVector2D to_target = TargetPos - p_vehicle_->p_owner_->transform_.pos_;

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
		speed = min(speed, p_vehicle_->max_speed_);

		// 여기서부터는 이미 to_target 벡터의 길이, dist를 계산했으므로
		// 이 벡터를 정규화할 필요가 없다는 것을 제외하고는
		// 찾기와 같은 방법으로 진행한다.
		CVector2D desired_velocity = to_target * speed / dist;

		return (desired_velocity - p_vehicle_->v_velocity_);
	}

	return CVector2D(0, 0);
}

// '추격하기(Pursuit)'은 에이전트의 미래 위치를 예측하여 그 지점으로 이동한다.
// 목표물을 가로챌 때 유용하다.
CVector2D CSteeringBehavior::Pursuit(const CVehicle* evader) {
	// 만약 도피자가 앞에 있고 에이전트를 대면하고 있다면, 도피자의
	// 현재 위치만을 찾을 수 있다.
	CVector2D to_evader = evader->p_owner_->transform_.pos_ - p_vehicle_->p_owner_->transform_.pos_;

	float relative_look = p_vehicle_->p_owner_->transform_.look_.Dot(evader->p_owner_->transform_.look_);

	if ((to_evader.Dot(p_vehicle_->p_owner_->transform_.look_) > 0) &&
		(relative_look < -0.95))  //acos(0.95)=18 degs
	{
		return Seek(evader->p_owner_->transform_.pos_);
	}

	// 바로 앞에 있지 않다고 간주되면, 도피자가 어디 있을 것인지를 예측한다.

	// 예측 시간은 도피자와 추격자 사이의 거리에 비례하고,
	// 이 에이전트들의 속도의 합에는 반비례한다.
	float look_ahead_time = to_evader.Length() /
		(p_vehicle_->max_speed_ + evader->Speed());

	// 이제 도피자가 있을 것으로 예측된 미래 위치로 찾아간다.
	return Seek(evader->p_owner_->transform_.pos_ + evader->v_velocity_ * look_ahead_time);
}

//this behavior attempts to evade a pursuer
CVector2D CSteeringBehavior::Evade(const CVehicle* pursuer) {
	/* Not necessary to include the check for facing direction this time */

	CVector2D ToPursuer = pursuer->p_owner_->transform_.pos_ - p_vehicle_->p_owner_->transform_.pos_;

	//uncomment the following two lines to have Evade only consider pursuers 
	//within a 'threat range'
	const float ThreatRange = 100.0;
	if (ToPursuer.LengthSq() > ThreatRange * ThreatRange) return CVector2D();

	//the lookahead time is propotional to the distance between the pursuer
	//and the pursuer; and is inversely proportional to the sum of the
	//agents' velocities
	float LookAheadTime = ToPursuer.Length() /
		(p_vehicle_->max_speed_ + pursuer->Speed());

	//now flee away from predicted future position of the pursuer
	return Flee(pursuer->p_owner_->transform_.pos_ + pursuer->v_velocity_ * LookAheadTime);
}

//this behavior makes the agent wander about randomly
CVector2D CSteeringBehavior::Wander() {
	/*this behavior is dependent on the update rate, so this line must
	be included when using time independent framerate.*/

	float JitterThisTimeSlice = 0.0f;  /*wander_jitter_ * WorldTimer*/

	/*first, add a small random vector to the target's position*/
	wander_target_ += CVector2D(RandomClamped() * JitterThisTimeSlice,
		RandomClamped() * JitterThisTimeSlice);

	//reproject this new vector back on to a unit circle
	wander_target_.Normalize();

	//increase the length of the vector to the same as the radius
	//of the wander circle
	wander_target_ *= wander_radius_;

	//move the target into a position WanderDist in front of the agent
	CVector2D target = wander_target_ + CVector2D(wander_distance_, 0);

	//project the target into world space
	CVector2D Target = PointToWorldSpace(target,
		p_vehicle_->p_owner_->transform_.look_,
		p_vehicle_->p_owner_->transform_.right_,
		p_vehicle_->p_owner_->transform_.pos_);

	//and steer towards it
	return Target - p_vehicle_->p_owner_->transform_.pos_;
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

	CVector2D pick_point = p_vehicle_->p_owner_->GameWorld()->PickPoint();

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

	if (On(wander))
	{
		v_steering_force_ += Wander() * weight_wander_;
	}

	v_steering_force_.Truncate(p_vehicle_->MaxForce());

	return v_steering_force_;
}

CVector2D CSteeringBehavior::CalcuclatePrioritized() {
	return CVector2D(0.0f, 0.0f);
}

CVector2D CSteeringBehavior::CalculateDithered() {
	return CVector2D(0.0f, 0.0f);
}
