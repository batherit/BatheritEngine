#pragma once
#include"CMathUtil.h"

class CTransformComponent {
public:
	CTransformComponent() :
		pos_(CVector2D(0.0f, 0.0f)),
		look_(CVector2D(0.0f, 1.0f)),
		right_(look_.Perp())
	{

	}
	~CTransformComponent();

	CVector2D pos_;
	CVector2D look_;
	CVector2D right_;
};

inline CVector2D PointToWorldSpace(const CVector2D &point,
	const CVector2D &AgentHeading,
	const CVector2D &AgentSide,
	const CVector2D &AgentPosition)
{
	//make a copy of the point
	CVector2D TransPoint = point;

	//create a transformation matrix
	C2DMatrix matTransform;

	//rotate
	matTransform.Rotate(AgentHeading, AgentSide);

	//and translate
	matTransform.Translate(AgentPosition.x_, AgentPosition.y_);

	//now transform the vertices
	matTransform.TransformVector2Ds(TransPoint);

	return TransPoint;
}