#pragma once
#include"CMathUtil.h"

class CObjectFuncTemplates
{
public:
	CObjectFuncTemplates();
	~CObjectFuncTemplates();
};

//----------------------- TagNeighbors ----------------------------------
//
//  단일 오브젝트 파라미터의 반지름 내에 있는 표준 컨테이너에 포함되는
//  오브젝트들을 표시해둡니다.
//------------------------------------------------------------------------
template <class T, class conT>
void TagNeighbors(const T& entity, conT& ContainerOfEntities, float radius)
{
	//범위 내 엔티티들을 검사합니다.
	for (typename conT::iterator curEntity = ContainerOfEntities.begin();
		curEntity != ContainerOfEntities.end();
		++curEntity)
	{
		//우선 현재의 태그를 클리어해줍니다.
		(*curEntity)->TagOff();

		CVector2D to = (*curEntity)->transform_.pos_ - entity->transform_.pos_;

		//the bounding radius of the other is taken into account by adding it 
		//to the range
		float range = radius + (*curEntity)->Mesh()->GetBoundingRad();

		// 범위 내의 엔터티 인 경우 추가 고려를 위해 태그를 지정합니다. 
		// (sqrts를 피하기 위해 indistance - square space 작동)
		if (((*curEntity) != entity) && (to.LengthSq() < range*range))
		{
			(*curEntity)->TagOn();
		}
	}//다음 오브젝트
}

//------------------------- Overlapped -----------------------------------
//
//  tests to see if an entity is overlapping any of a number of entities
//  stored in a std container
//------------------------------------------------------------------------
template <class T, class conT>
bool Overlapped(const T& ob, const conT& conOb, float MinDistBetweenObstacles = 40.0f)
{
	typename conT::const_iterator it;

	for (it = conOb.begin(); it != conOb.end(); ++it)
	{
		if (TwoCirclesOverlapped(ob->transform_.pos_,
			ob->Mesh()->GetBoundingRad() + MinDistBetweenObstacles,
			(*it)->transform_.pos_,
			(*it)->Mesh()->GetBoundingRad()))
		{
			return true;
		}
	}

	return false;
}

//----------------------------- TwoCirclesOverlapped ---------------------
//
//  Returns true if the two circles overlap
//------------------------------------------------------------------------
inline bool TwoCirclesOverlapped(CVector2D c1, float r1,
	CVector2D c2, float r2)
{
	float dist_between_centers = sqrt((c1.x_ - c2.x_) * (c1.x_ - c2.x_) +
		(c1.y_ - c2.y_) * (c1.y_ - c2.y_));

	if ((dist_between_centers < (r1 + r2)) || (dist_between_centers < fabs(r1 - r2)))
	{
		return true;
	}

	return false;
}

//-------------------- LineIntersection2D-------------------------
//
//	Given 2 lines in 2D space AB, CD this returns true if an 
//	intersection occurs and sets dist to the distance the intersection
//  occurs along AB. Also sets the 2d vector point to the point of
//  intersection
//----------------------------------------------------------------- 
inline bool LineIntersection2D(CVector2D   A,
	CVector2D   B,
	CVector2D   C,
	CVector2D   D,
	float&     dist,
	CVector2D&  point)
{

	float rTop = (A.y_ - C.y_)*(D.x_ - C.x_) - (A.x_ - C.x_)*(D.y_ - C.y_);
	float rBot = (B.x_ - A.x_)*(D.y_ - C.y_) - (B.y_ - A.y_)*(D.x_ - C.x_);

	float sTop = (A.y_ - C.y_)*(B.x_ - A.x_) - (A.x_ - C.x_)*(B.y_ - A.y_);
	float sBot = (B.x_ - A.x_)*(D.y_ - C.y_) - (B.y_ - A.y_)*(D.x_ - C.x_);

	if ((rBot == 0) || (sBot == 0))
	{
		//lines are parallel
		return false;
	}

	float r = rTop / rBot;
	float s = sTop / sBot;

	if ((r > 0) && (r < 1) && (s > 0) && (s < 1))
	{
		dist = Vec2DDistance(A, B) * r;

		point = A + r * (B - A);

		return true;
	}

	else
	{
		dist = 0;

		return false;
	}
}