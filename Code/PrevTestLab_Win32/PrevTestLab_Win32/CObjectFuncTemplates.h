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
//  ���� ������Ʈ �Ķ������ ������ ���� �ִ� ǥ�� �����̳ʿ� ���ԵǴ�
//  ������Ʈ���� ǥ���صӴϴ�.
//------------------------------------------------------------------------
template <class T, class conT>
void TagNeighbors(const T& entity, conT& ContainerOfEntities, float radius)
{
	//���� �� ��ƼƼ���� �˻��մϴ�.
	for (typename conT::iterator curEntity = ContainerOfEntities.begin();
		curEntity != ContainerOfEntities.end();
		++curEntity)
	{
		//�켱 ������ �±׸� Ŭ�������ݴϴ�.
		(*curEntity)->TagOff();

		CVector2D to = (*curEntity)->transform_.pos_ - entity->transform_.pos_;

		//the bounding radius of the other is taken into account by adding it 
		//to the range
		float range = radius + (*curEntity)->Mesh()->GetBoundingRad();

		// ���� ���� ����Ƽ �� ��� �߰� ����� ���� �±׸� �����մϴ�. 
		// (sqrts�� ���ϱ� ���� indistance - square space �۵�)
		if (((*curEntity) != entity) && (to.LengthSq() < range*range))
		{
			(*curEntity)->TagOn();
		}
	}//���� ������Ʈ
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