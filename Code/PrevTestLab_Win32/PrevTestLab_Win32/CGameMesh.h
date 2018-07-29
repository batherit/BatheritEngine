#pragma once
#include<vector>
#include "CMathUtil.h"

class CGameMesh
{
public:
	CGameMesh() : bounding_radius_(-1.0f) {}
	~CGameMesh();

	void SetMeshVB(std::vector<CVector2D> meshVB) { meshVB_ = meshVB; }
	void SetBoundingRad(float range) { bounding_radius_; }

	std::vector<CVector2D> GetMeshVB(void) { return meshVB_; }
	float GetBoundingRad(void) { return bounding_radius_; }

protected:
	std::vector<CVector2D> meshVB_;
	float bounding_radius_;
};

