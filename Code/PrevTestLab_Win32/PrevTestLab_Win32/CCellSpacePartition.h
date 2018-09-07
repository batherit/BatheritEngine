#pragma once
//-----------------------------------------------------------------------------
//
//  Name:   CellSpacePartition.h
//
//  Author: Mat Buckland (www.ai-junkie.com)
//
//  Desc:	게임 오브젝트를 포함하는 셀 그리드로 2D 공간을 나누는 클래스입니다.
//			게임 오브젝트를 가지고서 생성과 초기화가 진행되면, 빠른 근접 쿼리가
//			위치와 근접 반지름을 가지고 'CalculateNeighbors' 메서드를 호출함으로써
//			만들어질 수 있습니다.
//
//			게임 오브젝트가 움직일 수 있어서 셀 간 이동이 가능하다면 'Update' 메서드는
//			게임 오브젝트와 그것이 위치하는 셀 공간의 동기화를 위해 각 업데이트 사이클을
//			호출해야만 합니다.
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
//	게임 오브젝트로의 포인터 리스트를 포함하는 셀을 정의한다.
//------------------------------------------------------------------------
template <class entity>
struct Cell
{
	// 해당 셀에 존재하는 모든 엔티티들
	std::list<entity>    members_;

	// 셀의 바운딩 박스(윈도우즈의 디폴트 좌표계가 아래로 내려감으로써 증가하는 
	// y 축을 가지기 때문에 그것은 반전된다.)
	CInvertedAABBox2D     bbox_;

	Cell(CVector2D topleft,
		CVector2D botright) :bbox_(CInvertedAABBox2D(topleft, botright))
	{}
};

/////////// //////////////////////////////////////////////////////////////////
//  부분 분할 클래스
///////////////////////////////////////////////////////////////////////////////

template <class entity>
class CCellSpacePartition
{
public:
	CCellSpacePartition(float width,        // 환경의 너비
		float height,       // 환경의 높이
		int   cellsX,       // 수평축에 대한 셀의 수
		int   cellsY,       // 수직축에 대한 셀의 수
		int   MaxEntitys);  // 추가될 엔티티의 최대 수

	// 적절한 셀에 엔티티를 할당함으로써 클래스에 엔티티를 추가한다.
	inline void AddEntity(const entity& ent);

	// 'Update' 메서드로부터 해당 함수를 호출함으로써 엔티티의 셀을 갱신한다.
	inline void UpdateEntity(const entity& ent, CVector2D OldPos);

	// 이 메서드는 모든 타겟 이웃들을 계산하고 이웃 vector에 그들을
	// 저장한다. 해당 함수를 호출하고서 vector을 순회하기 위하여
	// begin, next 그리고 end 메서드를 사용하라. 
	inline void CalculateNeighbors(CVector2D TargetPos, float QueryRadius);

	// 이웃 벡터 맨 앞에 있는 엔티티에 대한 참조를 반환한다.
	inline entity& begin() { cur_neighbor_ = neighbors_.begin(); return *cur_neighbor_; }

	// 이웃 벡터에서 다음 엔티티를 참조 형태로 반환한다.
	inline entity& next() { ++cur_neighbor_; return *cur_neighbor_; }

	// 벡터의 끝이라면 true를 반환한다. (0 값은 끝을 표시한다.)
	inline bool   end() { return (cur_neighbor_ == neighbors_.end()) || (*cur_neighbor_ == 0); }

	// 각 셀의 멤버들을 클리어한다.
	void        EmptyCells();

	// 셀 엣지를 렌더하기 위해 gdi를 사용할 때 이것을 사용한다.
	inline void RenderCells()const;

private:
	// 게임 공간에서 위치를 고려하여, 이 메서드는 관련 셀 인덱스를 
	// 결정한다.
	inline int  PositionToIndex(const CVector2D& pos)const;
	// 공간에 요구된 셀들의 양
	std::vector<Cell<entity> >               cells_;

	// 에이전트가 이웃 공간을 찾을 때, 해당 변수는 유효한 이웃들을 
	// 저장하는데 쓰인다.
	std::vector<entity>                      neighbors_;

	// 아래의 이터레이터는 이웃을 나타내는 vector 순회하는데 쓰이는
	// next와 begin 메서드에 의해 사용될 것이다.
	typename std::vector<entity>::iterator   cur_neighbor_;

	// 엔티티가 존재하는 월드 공간의 폭과 높이
	float  space_width_;
	float  space_height_;

	// 공간이 분할될 셀의 수
	int    num_cells_X_;
	int    num_cells_Y_;

	float  cell_size_X_;
	float  cell_size_Y_;
};



//----------------------------- ctor ---------------------------------------
//--------------------------------------------------------------------------
template<class entity>
CCellSpacePartition<entity>::CCellSpacePartition(float  width,        // 2D 공간의 너비
	float  height,			// 2D 공간의 높이
	int    cellsX,			// 수평 축에 대한 분할 수
	int    cellsY,			// 수직 축에 대한 분할 수
	int    MaxEntitys) :	// 파티션을 최대 엔티티 수
	space_width_(width),
	space_height_(height),
	num_cells_X_(cellsX),
	num_cells_Y_(cellsY),
	neighbors_(MaxEntitys, entity())
{
	// 각 셀의 바운드를 계산한다.
	cell_size_X_ = width / cellsX;
	cell_size_Y_ = height / cellsY;

	// 셀을 생성한다.
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
//	이것은 이웃 vector를 생성하는데 호출되어야 한다. 이 메서드는
//	타겟 내 각 셀을 조사한다. 그 셀들이 엔티티들을 포함할 경우, 
//	엔티티들은 타겟의 이웃 지역 내 위치하고 있는지를 검사받는다.
//	만약 그렇다면, 그들은 이웃 리스트에 추가된다.
//------------------------------------------------------------------------
template<class entity>
void CCellSpacePartition<entity>::CalculateNeighbors(CVector2D TargetPos,
	float   QueryRadius)
{
	// 반복자를 생성하고 이것을 이웃 vector의 시작부에 위치시킨다.
	typename std::vector<entity>::iterator cur_neighbor = neighbors_.begin();

	// 타겟의 쿼리 영역 바운딩 박스인 쿼리 박스를 생성한다.
	CInvertedAABBox2D QueryBox(TargetPos - CVector2D(QueryRadius, QueryRadius),
		TargetPos + CVector2D(QueryRadius, QueryRadius));

	// 각 셀을 순회하며 셀의 바운딩 박스가 쿼리 박스에 겹치는지를 테스트한다.
	// 만약 그렇고 또한 엔티티를 포함한다면 미래 근접 테스트를 시행한다.
	typename std::vector<Cell<entity> >::iterator cur_cell;
	for (cur_cell = cells_.begin(); cur_cell != cells_.end(); ++cur_cell)
	{
		// 셀이 엔티티들을 포함하는지 그리고 이들이 쿼리 박스에 겹치는지를 검사한다.
		if (cur_cell->bbox_.IsOverlappedWith(QueryBox) &&
			!cur_cell->members_.empty())
		{
			// 이웃 리스트에 쿼리 박스 내 발견된 엔티티들을 추가한다.
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
	}// 다음 셀

	// 제로로 리스트의 끝을 표시한다.
	*cur_neighbor = 0;
}


//--------------------------- EmtpyCells --------------------------------------
//
//	각 셀들의 멤버를 클리어한다.
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
//	게임 월드 내 위치를 나타내는 2D vector가 주어질 때, 이 메서드는
//	적절한 셀로의 인덱스를 계산한다.
//------------------------------------------------------------------------
template<class entity>
inline int CCellSpacePartition<entity>::PositionToIndex(const CVector2D& pos)const
{
	int idx = (int)(num_cells_X_ * pos.x_ / space_width_) +
		((int)((num_cells_Y_)* pos.y_ / space_height_) * num_cells_X_);

	// 엔티티의 위치가 vector2d(space_width, space_height)와 같다면,
	// 인덱스는 초과할 것이다. 이것을 체크하고 조정할 필요가 있다.
	if (idx > (int)cells_.size() - 1) idx = (int)cells_.size() - 1;

	return idx;
}

//----------------------- AddEntity --------------------------------------
//
//	엔티티를 데이터 구조체에 추가하는데 사용된다.
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
//	엔티티가 셀을 이동했는지 확인한다. 그렇다면 데이터 구조체는 그에 맞춰
//	갱신된다.
//------------------------------------------------------------------------
template<class entity>
inline void CCellSpacePartition<entity>::UpdateEntity(const entity&  ent,
	CVector2D       OldPos)
{
	// 옛 위치와 새 위치가 같지 않다면 엔티티는 다른 셀로 이동한다.
	int old_idx = PositionToIndex(OldPos);
	int new_idx = PositionToIndex(ent->transform_.pos_);

	if (new_idx == old_idx) return;

	// 엔티티가 다른 셀로 이동했다면 현재 셀에서 삭제하고
	// 새로운 셀에 추가한다.
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