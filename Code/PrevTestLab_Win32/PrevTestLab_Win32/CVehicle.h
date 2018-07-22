#pragma once
#include"CPhysicsComponent.h"
#include"CSteeringBehaviors.h"

class CGameWorld;

class CVehicle : public CPhysicsComponent 
{
	friend class CSteeringBehavior;
public:
	CVehicle(float bounding_radius,
		CVector2D velocity,
		float max_speed,
		CVector2D look,
		float mass,
		float max_turn_rate,
		float max_force) : CPhysicsComponent(bounding_radius,
			velocity,
			max_speed,
			look,
			mass,
			max_turn_rate,
			max_force
			){
		p_steering_ = new CSteeringBehavior(this);
	}
	
	void Update(float time_elapsed);
	void Receive(const Telegram& r_msg) {};

private:
	
	CVehicle(const CVehicle&);
	CVehicle& operator=(const CVehicle&);
	~CVehicle();

	CSteeringBehavior * p_steering_;
};

