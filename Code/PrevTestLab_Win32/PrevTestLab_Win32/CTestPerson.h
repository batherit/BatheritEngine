#pragma once
#include "CVehicle.h"

class CTestPerson : public CVehicle
{
public:
	CTestPerson();
	CTestPerson(int ID,
		CVector2D velocity,
		float max_speed,
		CVector2D look,
		float mass,
		float max_turn_rate,
		float max_force) : CVehicle(
			ID,
			velocity,
			max_speed,
			look,
			mass,
			max_turn_rate,
			max_force) {}
	~CTestPerson();

private:
	// 상태를 추가.
};

