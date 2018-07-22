#pragma once
#include"CMathUtil.h"

class CTransformComponent {
public:
	CTransformComponent() :
		pos_(CVector2D(0.0f, 0.0f)),
		look_(CVector2D(0.0f, 1.0f)),
		right_(look_.Perp()),
		scale_(CVector2D(1.0f, 1.0f))
	{

	}
	~CTransformComponent();

	CVector2D pos_;
	CVector2D look_;
	CVector2D right_;
	CVector2D scale_;
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

//--------------------------- WorldTransform -----------------------------
//
//  given a std::vector of 2D vectors, a position, orientation and scale,
//  this function transforms the 2D vectors into the object's world space
//------------------------------------------------------------------------
inline std::vector<CVector2D> WorldTransform(std::vector<CVector2D> &points,
	const CVector2D   &pos,
	const CVector2D   &forward,
	const CVector2D   &side,
	const CVector2D   &scale)
{
	//copy the original vertices into the buffer about to be transformed
	std::vector<CVector2D> TranVector2Ds = points;

	//create a transformation matrix
	C2DMatrix matTransform;

	//scale
	if ((scale.x_ != 1.0) || (scale.y_ != 1.0))
	{
		matTransform.Scale(scale.x_, scale.y_);
	}

	//rotate
	matTransform.Rotate(forward, side);

	//and translate
	matTransform.Translate(pos.x_, pos.y_);

	//now transform the object's vertices
	matTransform.TransformVector2Ds(TranVector2Ds);

	return TranVector2Ds;
}

//--------------------------- WorldTransform -----------------------------
//
//  given a std::vector of 2D vectors, a position and  orientation
//  this function transforms the 2D vectors into the object's world space
//------------------------------------------------------------------------
inline std::vector<CVector2D> WorldTransform(std::vector<CVector2D> &points,
	const CVector2D   &pos,
	const CVector2D   &forward,
	const CVector2D   &side)
{
	//copy the original vertices into the buffer about to be transformed
	std::vector<CVector2D> TranVector2Ds = points;

	//create a transformation matrix
	C2DMatrix matTransform;

	//rotate
	matTransform.Rotate(forward, side);

	//and translate
	matTransform.Translate(pos.x_, pos.y_);

	//now transform the object's vertices
	matTransform.TransformVector2Ds(TranVector2Ds);

	return TranVector2Ds;
}