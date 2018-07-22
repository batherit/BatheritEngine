#include<algorithm>
#include"CUtil.h"
#include "CSteeringBehaviors.h"
#include "CVehicle.h"
using namespace std;

CSteeringBehavior::CSteeringBehavior(CVehicle* agent) {
	float theta = RandFloat() * TwoPi;
	wander_target_ = CVector2D(wander_radius_ * cos(theta), wander_radius_ * sin(theta));

	// TODO : Path = new Path();
	// TODO : Path->LoopOn();
}

CVector2D CSteeringBehavior::Calculate() {
	v_steering_force_.Zero();

	// TODO : 조종 행동을 변경

	return v_steering_force_ = CVector2D(100.0f, 100.0f);
}

//this behavior moves the agent towards a target position
CVector2D CSteeringBehavior::Seek(CVector2D TargetPos) {
	CVector2D DesiredVelocity = Vec2DNormalize(TargetPos - p_vehicle_->p_owner_->transform_.pos_)
		* p_vehicle_->max_speed_;

	return (DesiredVelocity - p_vehicle_->v_velocity_);
}

//this behavior returns a vector that moves the agent away
//from a target position
CVector2D CSteeringBehavior::Flee(CVector2D TargetPos) {
	//only flee if the target is within 'panic distance'. Work in distance
	//squared space.
	/* const double PanicDistanceSq = 100.0f * 100.0;
	if (Vec2DDistanceSq(m_pVehicle->Pos(), target) > PanicDistanceSq)
	{
	return Vector2D(0,0);
	}
	*/

	CVector2D DesiredVelocity = Vec2DNormalize(p_vehicle_->p_owner_->transform_.pos_ - TargetPos)
		* p_vehicle_->max_speed_;

	return (DesiredVelocity - p_vehicle_->v_velocity_);
}

//this behavior is similar to seek but it attempts to arrive 
//at the target position with a zero velocity
CVector2D CSteeringBehavior::Arrive(CVector2D     TargetPos, Deceleration deceleration) {
	CVector2D ToTarget = TargetPos - p_vehicle_->p_owner_->transform_.pos_;

	//calculate the distance to the target
	float dist = ToTarget.Length();

	if (dist > 0)
	{
		//because Deceleration is enumerated as an int, this value is required
		//to provide fine tweaking of the deceleration..
		const float DecelerationTweaker = 0.3f;

		//calculate the speed required to reach the target given the desired
		//deceleration
		float speed = dist / ((float)deceleration * DecelerationTweaker);

		//make sure the velocity does not exceed the max
		speed = min(speed, p_vehicle_->max_speed_);

		//from here proceed just like Seek except we don't need to normalize 
		//the ToTarget vector because we have already gone to the trouble
		//of calculating its length: dist. 
		CVector2D DesiredVelocity = ToTarget * speed / dist;

		return (DesiredVelocity - p_vehicle_->v_velocity_);
	}

	return CVector2D(0, 0);
}

//this behavior predicts where an agent will be in time T and seeks
//towards that point to intercept it.
CVector2D CSteeringBehavior::Pursuit(const CVehicle* evader) {
	//if the evader is ahead and facing the agent then we can just seek
	//for the evader's current position.
	CVector2D ToEvader = evader->p_owner_->transform_.pos_ - p_vehicle_->p_owner_->transform_.pos_;

	float RelativeHeading = p_vehicle_->p_owner_->transform_.look_.Dot(evader->p_owner_->transform_.look_);

	if ((ToEvader.Dot(p_vehicle_->p_owner_->transform_.look_) > 0) &&
		(RelativeHeading < -0.95))  //acos(0.95)=18 degs
	{
		return Seek(evader->p_owner_->transform_.pos_);
	}

	//Not considered ahead so we predict where the evader will be.

	//the lookahead time is propotional to the distance between the evader
	//and the pursuer; and is inversely proportional to the sum of the
	//agent's velocities
	float LookAheadTime = ToEvader.Length() /
		(p_vehicle_->max_speed_ + evader->Speed());

	//now seek to the predicted future position of the evader
	return Seek(evader->p_owner_->transform_.pos_ + evader->v_velocity_ * LookAheadTime);
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