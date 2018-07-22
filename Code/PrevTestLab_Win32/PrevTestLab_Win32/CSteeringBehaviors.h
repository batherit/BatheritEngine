#pragma once

#include"CMathUtil.h"
//#include"CVehicle.h"

class CVehicle;

class CSteeringBehavior
{
public:
	CSteeringBehavior(CVehicle* agent);
	~CSteeringBehavior();
	CVector2D Calculate();

private:
	// 조종 행동이 조종하는 비하이클
	CVehicle* p_vehicle_;
	CVector2D v_steering_force_;

	CVehicle* p_target_agent1_;
	CVehicle* p_target_agent2_;
	CVector2D v_target_;

	CVector2D wander_target_;

	//explained above
	float wander_jitter_;
	float wander_radius_;
	float wander_distance_;

	enum Deceleration { slow = 3, normal = 2, fast = 1 };

	//default
	Deceleration m_Deceleration;

	//this behavior moves the agent towards a target position
	CVector2D Seek(CVector2D TargetPos);

	//this behavior returns a vector that moves the agent away
	//from a target position
	CVector2D Flee(CVector2D TargetPos);

	//this behavior is similar to seek but it attempts to arrive 
	//at the target position with a zero velocity
	CVector2D Arrive(CVector2D     TargetPos, Deceleration deceleration);

	//this behavior predicts where an agent will be in time T and seeks
	//towards that point to intercept it.
	CVector2D Pursuit(const CVehicle* agent);

	//this behavior attempts to evade a pursuer
	CVector2D Evade(const CVehicle* agent);

	//this behavior makes the agent wander about randomly
	CVector2D Wander();
};

