#pragma once
#include"CPhysicsComponent.h"

class CGameWorld;
class CSteeringBehavior;

class CVehicle : public CPhysicsComponent 
{
	friend class CSteeringBehavior;
public:
	void Update(float time_elapsed);

private:
	CVehicle(const CVehicle&);
	CVehicle& operator=(const CVehicle&);
	~CVehicle();

	CSteeringBehavior * p_steering_;
};

