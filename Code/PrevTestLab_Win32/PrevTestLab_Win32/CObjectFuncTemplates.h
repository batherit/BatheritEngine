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