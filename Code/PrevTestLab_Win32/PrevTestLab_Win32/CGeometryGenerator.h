#pragma once
#include <vector>
#include "CMathUtil.h"

class CGeometryGenerator
{
public:
	CGeometryGenerator();
	~CGeometryGenerator();
};

inline std::vector<CVector2D> VehicleVB(void) {
	std::vector<CVector2D> vehicleVB;
	const int NumVehicleVerts = 3;

	CVector2D vehicle[NumVehicleVerts] = { CVector2D(-1.0f,0.6f),
		CVector2D(1.0f,0.0f),
		CVector2D(-1.0f,-0.6f) };

	for (int vtx = 0; vtx<NumVehicleVerts; ++vtx)
	{
		vehicleVB.push_back(vehicle[vtx]);
	}

	return vehicleVB;
}