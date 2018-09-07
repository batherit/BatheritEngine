#pragma once
//-----------------------------------------------------------------------------
//
//  Name:   CellSpacePartition.h
//
//  Author: Mat Buckland (www.ai-junkie.com)
//
//  Desc:	���� ������Ʈ�� �����ϴ� �� �׸���� 2D ������ ������ Ŭ�����Դϴ�.
//			���� ������Ʈ�� ������ ������ �ʱ�ȭ�� ����Ǹ�, ���� ���� ������
//			��ġ�� ���� �������� ������ 'CalculateNeighbors' �޼��带 ȣ�������ν�
//			������� �� �ֽ��ϴ�.
//
//			���� ������Ʈ�� ������ �� �־ �� �� �̵��� �����ϴٸ� 'Update' �޼����
//			���� ������Ʈ�� �װ��� ��ġ�ϴ� �� ������ ����ȭ�� ���� �� ������Ʈ ����Ŭ��
//			ȣ���ؾ߸� �մϴ�.
//
//-----------------------------------------------------------------------------
#pragma warning (disable:4786)

#include <vector>
#include <list>
#include <cassert>

#include "CVector2D.h"
#include "CInvertedAABBox2D.h"



//------------------------------------------------------------------------
//
//	���� ������Ʈ���� ������ ����Ʈ�� �����ϴ� ���� �����Ѵ�.
//------------------------------------------------------------------------
template <class entity>
struct Cell
{
	// �ش� ���� �����ϴ� ��� ��ƼƼ��
	std::list<entity>    members_;

	// ���� �ٿ�� �ڽ�(���������� ����Ʈ ��ǥ�谡 �Ʒ��� ���������ν� �����ϴ� 
	// y ���� ������ ������ �װ��� �����ȴ�.)
	CInvertedAABBox2D     bbox_;

	Cell(CVector2D topleft,
		CVector2D botright) :bbox_(CInvertedAABBox2D(topleft, botright))
	{}
};

/////////// //////////////////////////////////////////////////////////////////
//  �κ� ���� Ŭ����
///////////////////////////////////////////////////////////////////////////////

template <class entity>
class CCellSpacePartition
{
public:
	CCellSpacePartition(float width,        // ȯ���� �ʺ�
		float height,       // ȯ���� ����
		int   cellsX,       // �����࿡ ���� ���� ��
		int   cellsY,       // �����࿡ ���� ���� ��
		int   MaxEntitys);  // �߰��� ��ƼƼ�� �ִ� ��

	// ������ ���� ��ƼƼ�� �Ҵ������ν� Ŭ������ ��ƼƼ�� �߰��Ѵ�.
	inline void AddEntity(const entity& ent);

	// 'Update' �޼���κ��� �ش� �Լ��� ȣ�������ν� ��ƼƼ�� ���� �����Ѵ�.
	inline void UpdateEntity(const entity& ent, CVector2D OldPos);

	// �� �޼���� ��� Ÿ�� �̿����� ����ϰ� �̿� vector�� �׵���
	// �����Ѵ�. �ش� �Լ��� ȣ���ϰ� vector�� ��ȸ�ϱ� ���Ͽ�
	// begin, next �׸��� end �޼��带 ����϶�. 
	inline void CalculateNeighbors(CVector2D TargetPos, float QueryRadius);

	// �̿� ���� �� �տ� �ִ� ��ƼƼ�� ���� ������ ��ȯ�Ѵ�.
	inline entity& begin() { cur_neighbor_ = neighbors_.begin(); return *cur_neighbor_; }

	// �̿� ���Ϳ��� ���� ��ƼƼ�� ���� ���·� ��ȯ�Ѵ�.
	inline entity& next() { ++cur_neighbor_; return *cur_neighbor_; }

	// ������ ���̶�� true�� ��ȯ�Ѵ�. (0 ���� ���� ǥ���Ѵ�.)
	inline bool   end() { return (cur_neighbor_ == neighbors_.end()) || (*cur_neighbor_ == 0); }

	// �� ���� ������� Ŭ�����Ѵ�.
	void        EmptyCells();

	// �� ������ �����ϱ� ���� gdi�� ����� �� �̰��� ����Ѵ�.
	inline void RenderCells()const;

private:
	// ���� �������� ��ġ�� ����Ͽ�, �� �޼���� ���� �� �ε����� 
	// �����Ѵ�.
	inline int  PositionToIndex(const CVector2D& pos)const;
	// ������ �䱸�� ������ ��
	std::vector<Cell<entity> >               cells_;

	// ������Ʈ�� �̿� ������ ã�� ��, �ش� ������ ��ȿ�� �̿����� 
	// �����ϴµ� ���δ�.
	std::vector<entity>                      neighbors_;

	// �Ʒ��� ���ͷ����ʹ� �̿��� ��Ÿ���� vector ��ȸ�ϴµ� ���̴�
	// next�� begin �޼��忡 ���� ���� ���̴�.
	typename std::vector<entity>::iterator   cur_neighbor_;

	// ��ƼƼ�� �����ϴ� ���� ������ ���� ����
	float  space_width_;
	float  space_height_;

	// ������ ���ҵ� ���� ��
	int    num_cells_X_;
	int    num_cells_Y_;

	float  cell_size_X_;
	float  cell_size_Y_;
};



//----------------------------- ctor ---------------------------------------
//--------------------------------------------------------------------------
template<class entity>
CCellSpacePartition<entity>::CCellSpacePartition(float  width,        // 2D ������ �ʺ�
	float  height,			// 2D ������ ����
	int    cellsX,			// ���� �࿡ ���� ���� ��
	int    cellsY,			// ���� �࿡ ���� ���� ��
	int    MaxEntitys) :	// ��Ƽ���� �ִ� ��ƼƼ ��
	space_width_(width),
	space_height_(height),
	num_cells_X_(cellsX),
	num_cells_Y_(cellsY),
	neighbors_(MaxEntitys, entity())
{
	// �� ���� �ٿ�带 ����Ѵ�.
	cell_size_X_ = width / cellsX;
	cell_size_Y_ = height / cellsY;

	// ���� �����Ѵ�.
	for (int y = 0; y< num_cells_Y_; ++y)
	{
		for (int x = 0; x< num_cells_X_; ++x)
		{
			float left = x * cell_size_X_;
			float right = left + cell_size_X_;
			float top = y * cell_size_Y_;
			float bot = top + cell_size_Y_;

			cells_.push_back(Cell<entity>(Vector2D(left, top), Vector2D(right, bot)));
		}
	}
}

//----------------------- CalculateNeighbors ----------------------------
//
//	�̰��� �̿� vector�� �����ϴµ� ȣ��Ǿ�� �Ѵ�. �� �޼����
//	Ÿ�� �� �� ���� �����Ѵ�. �� ������ ��ƼƼ���� ������ ���, 
//	��ƼƼ���� Ÿ���� �̿� ���� �� ��ġ�ϰ� �ִ����� �˻�޴´�.
//	���� �׷��ٸ�, �׵��� �̿� ����Ʈ�� �߰��ȴ�.
//------------------------------------------------------------------------
template<class entity>
void CCellSpacePartition<entity>::CalculateNeighbors(CVector2D TargetPos,
	float   QueryRadius)
{
	// �ݺ��ڸ� �����ϰ� �̰��� �̿� vector�� ���ۺο� ��ġ��Ų��.
	typename std::vector<entity>::iterator cur_neighbor = neighbors_.begin();

	// Ÿ���� ���� ���� �ٿ�� �ڽ��� ���� �ڽ��� �����Ѵ�.
	CInvertedAABBox2D QueryBox(TargetPos - CVector2D(QueryRadius, QueryRadius),
		TargetPos + CVector2D(QueryRadius, QueryRadius));

	// �� ���� ��ȸ�ϸ� ���� �ٿ�� �ڽ��� ���� �ڽ��� ��ġ������ �׽�Ʈ�Ѵ�.
	// ���� �׷��� ���� ��ƼƼ�� �����Ѵٸ� �̷� ���� �׽�Ʈ�� �����Ѵ�.
	typename std::vector<Cell<entity> >::iterator cur_cell;
	for (cur_cell = cells_.begin(); cur_cell != cells_.end(); ++cur_cell)
	{
		// ���� ��ƼƼ���� �����ϴ��� �׸��� �̵��� ���� �ڽ��� ��ġ������ �˻��Ѵ�.
		if (cur_cell->bbox_.IsOverlappedWith(QueryBox) &&
			!cur_cell->members_.empty())
		{
			// �̿� ����Ʈ�� ���� �ڽ� �� �߰ߵ� ��ƼƼ���� �߰��Ѵ�.
			typename std::list<entity>::iterator it = cur_cell->members_.begin();
			for (it; it != cur_cell->members_.end(); ++it)
			{
				if (Vec2DDistanceSq((*it)->transform_.pos_, TargetPos) <
					QueryRadius*QueryRadius)
				{
					*cur_neighbor++ = *it;
				}
			}
		}
	}// ���� ��

	// ���η� ����Ʈ�� ���� ǥ���Ѵ�.
	*cur_neighbor = 0;
}


//--------------------------- EmtpyCells --------------------------------------
//
//	�� ������ ����� Ŭ�����Ѵ�.
//------------------------------------------------------------------------
template<class entity>
void CCellSpacePartition<entity>::EmptyCells()
{
	typename std::vector<Cell<entity>>::iterator it = cells_.begin();

	for (it; it != cells_.end(); ++it)
	{
		it->members_.clear();
	}
}

//--------------------- PositionToIndex ----------------------------------
//
//	���� ���� �� ��ġ�� ��Ÿ���� 2D vector�� �־��� ��, �� �޼����
//	������ ������ �ε����� ����Ѵ�.
//------------------------------------------------------------------------
template<class entity>
inline int CCellSpacePartition<entity>::PositionToIndex(const CVector2D& pos)const
{
	int idx = (int)(num_cells_X_ * pos.x_ / space_width_) +
		((int)((num_cells_Y_)* pos.y_ / space_height_) * num_cells_X_);

	// ��ƼƼ�� ��ġ�� vector2d(space_width, space_height)�� ���ٸ�,
	// �ε����� �ʰ��� ���̴�. �̰��� üũ�ϰ� ������ �ʿ䰡 �ִ�.
	if (idx > (int)cells_.size() - 1) idx = (int)cells_.size() - 1;

	return idx;
}

//----------------------- AddEntity --------------------------------------
//
//	��ƼƼ�� ������ ����ü�� �߰��ϴµ� ���ȴ�.
//------------------------------------------------------------------------
template<class entity>
inline void CCellSpacePartition<entity>::AddEntity(const entity& ent)
{
	assert(ent);

	int sz = cells_.size();
	int idx = PositionToIndex(ent->transform_.pos_);

	cells_[idx].members_.push_back(ent);
}

//----------------------- UpdateEntity -----------------------------------
//
//	��ƼƼ�� ���� �̵��ߴ��� Ȯ���Ѵ�. �׷��ٸ� ������ ����ü�� �׿� ����
//	���ŵȴ�.
//------------------------------------------------------------------------
template<class entity>
inline void CCellSpacePartition<entity>::UpdateEntity(const entity&  ent,
	CVector2D       OldPos)
{
	// �� ��ġ�� �� ��ġ�� ���� �ʴٸ� ��ƼƼ�� �ٸ� ���� �̵��Ѵ�.
	int old_idx = PositionToIndex(OldPos);
	int new_idx = PositionToIndex(ent->transform_.pos_);

	if (new_idx == old_idx) return;

	// ��ƼƼ�� �ٸ� ���� �̵��ߴٸ� ���� ������ �����ϰ�
	// ���ο� ���� �߰��Ѵ�.
	cells_[old_idx].members_.remove(ent);
	cells_[new_idx].members_.push_back(ent);
}

//-------------------------- RenderCells -----------------------------------
//--------------------------------------------------------------------------
template<class entity>
inline void CCellSpacePartition<entity>::RenderCells()const
{
	typename std::vector<Cell<entity>>::const_iterator cur_cell;
	for (cur_cell = cells_.begin(); cur_cell != cells_.end(); ++cur_cell)
	{
		cur_cell->bbox_.Render(false);
	}
}