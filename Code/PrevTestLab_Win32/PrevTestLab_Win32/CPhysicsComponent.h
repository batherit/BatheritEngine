#pragma once
#include<cassert>
#include"CMathUtil.h"
#include"CGameObject.h"
#include"CGameComponent.h"

class CPhysicsComponent : public CGameComponent {
public:
	CPhysicsComponent(
		CVector2D velocity,
		float max_speed,
		CVector2D look,
		float mass,
		float max_turn_rate,
		float max_force) :
		/*v_look_(look),*/
		v_velocity_(velocity),
		mass_(mass),
		/*v_right_(v_look_.Perp()),*/
		max_speed_(max_speed),
		max_turn_rate_(max_turn_rate),
		max_force_(max_force) {}
	virtual ~CPhysicsComponent() {}
	
	//접근자 목록
	CVector2D Velocity()const { return v_velocity_; }
	void SetVelocity(const CVector2D& new_velocity) { v_velocity_ = new_velocity; }

	float Mass()const { return mass_; }

	/*CVector2D Right()const { return v_right_; }*/

	float MaxSpeed()const { return max_speed_; }
	void SetMaxSpeed(float new_max_speed) { max_speed_ = new_max_speed; }

	float MaxForce()const { return max_force_; }
	void SetMaxForce(float max_force) { max_force_ = max_force; }

	bool IsSpeedMaxedOut()const { return max_speed_ * max_speed_ >= v_velocity_.LengthSq(); }
	float Speed()const { return v_velocity_.Length(); }
	float SpeedSq()const { return v_velocity_.LengthSq(); }

	void SetLook(CVector2D new_look);
	bool RotateLookToFacePosition(CVector2D target);

	float MaxTurnRate()const { return max_turn_rate_; }
	void SetMaxTurnRate(float max_turn_rate) { max_turn_rate_ = max_turn_rate; }

protected:
	CVector2D v_velocity_;

	float mass_;
	float max_speed_;
	float max_force_;
	float max_turn_rate_;
};

inline void CPhysicsComponent::SetLook(CVector2D new_look)
{
	assert((new_look.LengthSq() - 1.0) < 1E-5);

	p_owner_->transform_.look_ = new_look;

	p_owner_->transform_.right_ = new_look.Perp();
}

inline bool CPhysicsComponent::RotateLookToFacePosition(CVector2D target)
{
	CVector2D toTarget = Vec2DNormalize(target - p_owner_->transform_.pos_);

	//first determine the angle between the heading vector and the target
	float angle = acos(p_owner_->transform_.look_.Dot(toTarget));

	//return true if the player is facing the target
	if (angle < 1E-5) return true;

	//clamp the amount to turn to the max turn rate
	if (angle > max_turn_rate_) angle = max_turn_rate_;

	//The next few lines use a rotation matrix to rotate the player's heading
	//vector accordingly
	C2DMatrix RotationMatrix;

	//notice how the direction of rotation has to be determined when creating
	//the rotation matrix
	RotationMatrix.Rotate(angle * p_owner_->transform_.look_.Sign(toTarget));
	RotationMatrix.TransformVector2Ds(p_owner_->transform_.look_);
	RotationMatrix.TransformVector2Ds(v_velocity_);

	//finally recreate m_vSide
	p_owner_->transform_.right_ = p_owner_->transform_.look_.Perp();

	return false;
}