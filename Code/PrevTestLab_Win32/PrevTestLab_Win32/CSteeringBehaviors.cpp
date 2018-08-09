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

	// TODO : ���� ���� ����
	// TODO : ���� �ൿ ���� ����
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
	
	// �д� ����(���� �Ÿ� �Ǻ�)�� ���� ������ '�޾Ƴ���(Flee)'�� ��ȿ�ϵ��� �Ѵ�.
	const double panic_distance_sq = 100.0f * 100.0f;
	if (Vec2DDistanceSq(p_vehicle_->transform_.pos_, target_pos) > panic_distance_sq) {
		return CVector2D(0,0);
	}
	

	CVector2D desired_velocity = Vec2DNormalize(p_vehicle_->transform_.pos_ - target_pos)
		* p_vehicle_->physics_->MaxSpeed();

	return (desired_velocity - p_vehicle_->physics_->Velocity());
}

// ��ǥ�� �����ϱ� ���� �������� �ִ� 'ã��(Seek)'�ൿ�̶� ���� �� ���̴�.
CVector2D CSteeringBehavior::Arrive(CVector2D     TargetPos, Deceleration deceleration) {
	CVector2D to_target = TargetPos - p_vehicle_->transform_.pos_;

	//calculate the distance to the target
	float dist = to_target.Length();

	if (dist > 0)
	{
		// Deceleration(����)�� ���ŵ� �����̹Ƿ�, �̼������� ������
		// �����ϱ� ���� �� ���� �䱸�ȴ�.
		const float deceleration_tweaker = 0.3f;

		// ���ϴ� ������ �־��� ��� ��ǥ�� �����ϱ� ���� �䱸�Ǵ� �ӵ��� ����Ѵ�.
		float speed = dist / ((float)deceleration * deceleration_tweaker);

		// �ӵ��� �ִ밪�� ���� �ʴ����� Ȯ���Ѵ�.
		speed = min(speed, p_vehicle_->physics_->MaxSpeed());

		// ���⼭���ʹ� �̹� to_target ������ ����, dist�� ��������Ƿ�
		// �� ���͸� ����ȭ�� �ʿ䰡 ���ٴ� ���� �����ϰ��
		// ã��� ���� ������� �����Ѵ�.
		CVector2D desired_velocity = to_target * speed / dist;

		return (desired_velocity - p_vehicle_->physics_->Velocity());
	}

	return CVector2D(0, 0);
}

// '�߰��ϱ�(Pursuit)'�� ������Ʈ�� �̷� ��ġ�� �����Ͽ� �� �������� �̵��Ѵ�.
// ��ǥ���� ����ç �� �����ϴ�.
CVector2D CSteeringBehavior::Pursuit(const CVehicle* evader) {
	// ���� �����ڰ� �տ� �ְ� ������Ʈ�� ����ϰ� �ִٸ�, ��������
	// ���� ��ġ���� ã�� �� �ִ�.
	CVector2D to_evader = evader->transform_.pos_ - p_vehicle_->transform_.pos_;

	float relative_look = p_vehicle_->transform_.look_.Dot(evader->transform_.look_);

	if ((to_evader.Dot(p_vehicle_->transform_.look_) > 0) &&
		(relative_look < -0.95))  //acos(0.95)=18 degs
	{
		return Seek(evader->transform_.pos_);
	}

	// �ٷ� �տ� ���� �ʴٰ� ���ֵǸ�, �����ڰ� ��� ���� �������� �����Ѵ�.

	// ���� �ð��� �����ڿ� �߰��� ������ �Ÿ��� ����ϰ�,
	// �� ������Ʈ���� �ӵ��� �տ��� �ݺ���Ѵ�.
	float look_ahead_time = to_evader.Length() /
		(p_vehicle_->physics_->MaxSpeed() + evader->physics_->Speed());

	// ���� �����ڰ� ���� ������ ������ �̷� ��ġ�� ã�ư���.
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

	// �켱 �ҷ��� ������ ���͸� ��ǥ���� ��ġ�� ���Ѵ�. (RandomClamped��
	// -1�� 1 ������ ���� ��ȯ�Ѵ�.
	wander_target_ += CVector2D(RandomClamped() * jitter_this_time_slice,
		RandomClamped() * jitter_this_time_slice);

	// �� ���ο� ���͸� �ٽ� ���������� �������Ѵ�.
	wander_target_.Normalize();

	// ������ ���̸� ��ȸ ���� �ݰ�� �����ϰ� ������Ų��.
	wander_target_ *= wander_radius_;

	// ��ǥ�� ������Ʈ ���� wander_distance_ ��ġ�� �̵���Ų��.
	CVector2D target_local = wander_target_ + CVector2D(wander_distance_, 0);

	// ��ǥ�� ���� �������� �����Ѵ�. Local -> World
	CVector2D target_world = PointToWorldSpace(target_local,
		p_vehicle_->transform_.look_,
		p_vehicle_->transform_.right_,
		p_vehicle_->transform_.pos_);

	// �׸��� �������� �����ذ���.
	return target_world - p_vehicle_->transform_.pos_;
}

CVector2D CSteeringBehavior::ObstacleAvoidance(const std::vector<CGameObject*>& obstacles) {
	// ���� ������ ���̴� ������Ʈ�� �ӵ��� ����Ѵ�.
	detection_box_length_ = VehiclePrm.min_detection_box_length_ +
		(p_vehicle_->physics_->Speed() / p_vehicle_->physics_->MaxSpeed()) *
		VehiclePrm.min_detection_box_length_;

	// ���� ���� ���� ���� ��ֹ����� ó���ϱ� ���� ǥ���صд�.
	p_vehicle_->GameWorld()->TagObstaclesWithinViewRange(p_vehicle_, detection_box_length_);

	// �� �κ��� �����ϰ� �ִ� ���� ������ ��ֹ�(CIB)���� �����Ѵ�.
	CGameObject* closest_intersecting_obstacle = nullptr;

	// �� �κ��� CIB������ �Ÿ��� �����ϴµ� ���ȴ�.
	float dist_to_closest_IP = MaxFloat;

	// TODO : LocalPosOfClosestObstacle...2018/07/29
	
	return CVector2D(0.0f, 0.0f);
}

//---------------------- CalculateWeightedSum ----------------------------
//
// �̰��� �ܼ��� ��� 'Ȱ��ȭ�� ���۵� * ����ġ'�� ���ϰ� 
// ����� ��ȯ�ϱ� ���� �ִ�� �̿��� �� �ִ� ���������� ��ġ�� �ڸ��ϴ�.
//------------------------------------------------------------------------
CVector2D CSteeringBehavior::CalculateWeightedSum() {
	if (On(wall_avoidance)) {
		//TODO : wall_aboidance�� ���� �ൿ ����
	}

	if (On(obstacle_avoidance)) {
		//TODO : obstacle_avoidance�� ���� �ൿ ����
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
