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

//--------------------- PointToLocalSpace --------------------------------
//
//------------------------------------------------------------------------
inline CVector2D PointToLocalSpace(const CVector2D &point,
	CVector2D &agent_heading,
	CVector2D &agent_side,
	CVector2D &agent_position)
{

	// 포인트에 대한 카피본 생성
	CVector2D trans_point = point;

	// 트랜스폼 행렬을 생성
	C2DMatrix mat_transform;

	float Tx = -agent_position.Dot(agent_heading);
	float Ty = -agent_position.Dot(agent_side);

	// 트랜스폼 행렬에 데이터 저장
	mat_transform._11(agent_heading.x_); mat_transform._12(agent_side.x_);
	mat_transform._21(agent_heading.y_); mat_transform._22(agent_side.y_);
	mat_transform._31(Tx);           mat_transform._32(Ty);

	// 정점 변환
	mat_transform.TransformVector2Ds(trans_point);

	return trans_point;
}

//--------------------- VectorToWorldSpace --------------------------------
//
//  에이전트의 로컬 공간에서 월드 공간으로 벡터를 변환한다.
//------------------------------------------------------------------------
inline CVector2D VectorToWorldSpace(const CVector2D &vec,
	const CVector2D &agent_look,
	const CVector2D &agent_right)
{
	// 포인트에 대한 복사본을 생성
	CVector2D trans_vec = vec;

	// 트랜스폼 행렬 생성
	C2DMatrix mat_transform;

	// 회전
	mat_transform.Rotate(agent_look, agent_right);

	// 정점 변환
	mat_transform.TransformVector2Ds(trans_vec);

	return trans_vec;
}

//-------------------------- Vec2DRotateAroundOrigin --------------------------
//
//  rotates a vector ang rads around the origin
//-----------------------------------------------------------------------------
inline void Vec2DRotateAroundOrigin(CVector2D& v, float ang)
{
	//create a transformation matrix
	C2DMatrix mat;

	//rotate
	mat.Rotate(ang);

	//now transform the object's vertices
	mat.TransformVector2Ds(v);
}