#pragma once
#include"CMathUtil.h"

class CTransformComponent {
	friend class CPhysicsComponent;

public:
	CVector2D pos_;
	CTransformComponent() : 
		pos_(CVector2D(0.0f, 0.0f)),
		look_(CVector2D(0.0f, 1.0f)),
		right_(look_.Perp())
	{

	}
	~CTransformComponent();

private:
	CVector2D look_;
	CVector2D right_;
};

