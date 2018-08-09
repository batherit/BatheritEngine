#pragma once
#include<cassert>
#include"CMathUtil.h"
#include"CGameComponent.h"

class CPhysicsComponent : public CGameComponent {
public:
	CPhysicsComponent() :
		v_velocity_(CVector2D(0.0f, 0.0f)),
		mass_(0.0f),
		max_speed_(0.0f),
		max_turn_rate_(0.0f),
		max_force_(0.0f) {}

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

	void Receive(const Telegram& r_msg) {};	// CGameObject가 호출
	void Update(float elapsed_time) {};		// SendMessage 호출 가능, CGameObject가 호출

protected:
	CVector2D v_velocity_;

	float mass_;
	float max_speed_;
	float max_force_;
	float max_turn_rate_;
};