#pragma once
#include "CGameObject.h"
#include"CPhysicsComponent.h"
#include"CSteeringBehaviors.h"

class CGameWorld;

class CVehicle : public CGameObject
{
	friend class CSteeringBehavior;
public:
	CVehicle(
		int ID,
		CVector2D velocity,
		float max_speed,
		CVector2D look,
		float mass,
		float max_turn_rate,
		float max_force) : CGameObject(ID)
	{
		physics_ = new CPhysicsComponent(
			velocity,
			max_speed,
			look,
			mass,
			max_turn_rate,
			max_force
		);
		physics_->SetOwner(this);
		p_steering_ = new CSteeringBehavior(this);
	}
	~CVehicle();

	void Update(float time_elapsed);
	void Receive(const Telegram& r_msg) {};
	CSteeringBehavior*const  Steering()const { return p_steering_; }

private:
	CVehicle(const CVehicle&);
	CVehicle& operator=(const CVehicle&);
	CSteeringBehavior * p_steering_;

	bool is_tagged_;
};

