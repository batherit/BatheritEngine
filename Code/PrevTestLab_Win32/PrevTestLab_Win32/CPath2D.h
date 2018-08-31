#pragma once
#include<list>
#include<cassert>
#include "CVector2D.h"

class CPath2D
{
public:
	CPath2D() : is_looped_(false) {}

	// 무작위 웨이 포인트들을 가지고서 경로를 만들어내는 생성자.
	// MinX/Y 그리고 MaxX/Y는 경로의 바운딩 박스를 정의한다.
	CPath2D(int num_waypoints,
		float min_X,
		float min_Y,
		float max_X,
		float max_Y,
		bool looped) : is_looped_(looped) {
		CreateRandomPath(num_waypoints, min_X, min_Y, max_X, max_Y);
		cur_waypoint_ = waypoints_.begin();
	}
	~CPath2D();

	CVector2D CurrentWaypoint()const { return *cur_waypoint_; }

	// 리스트의 끝에 도달했다면 true를 반환한다.
	bool Finished() { return !(cur_waypoint_ != waypoints_.end()); }

	// 리스트에서 다음 웨이포인트로 이터레이터를 이동시킨다.
	inline void SetNextWaypoint();

	// min/max 변수로 정의된 직사각형에 둘러싸인 무작위 경로를 만들어낸다.
	std::list<CVector2D> CreateRandomPath(
		int num_waypoints_,
		float min_X,
		float min_Y,
		float max_X,
		float max_Y);

	void LoopOn() { is_looped_ = true; }
	void LoopOff() { is_looped_ = false; }

	// 경로의 끝에 웨이포인트를 추가한다.
	void AddWaypoint(CVector2D new_point);

	// 다른 경로나 벡터 리스트를 갖고서 경로를 설정하는 메서드
	void Set(std::list<CVector2D> new_path) { waypoints_ = new_path; cur_waypoint_ = waypoints_.begin(); }
	void Set(const CPath2D& path) { waypoints_ = path.GetPath(); cur_waypoint_ = waypoints_.begin(); }

	void Clear() { waypoints_.clear(); }
	std::list<CVector2D> GetPath() const { return waypoints_; }

	// 경로를 렌더링한다.
	void Render() const;

private:
	std::list<CVector2D> waypoints_;
	std::list<CVector2D>::iterator cur_waypoint_;
	bool is_looped_;
};

//-------------------- Methods -------------------------------------------

inline void CPath2D::SetNextWaypoint() {
	assert(waypoints_.size() > 0);

	if (++cur_waypoint_ == waypoints_.end()) {
		if (is_looped_) {
			cur_waypoint_ = waypoints_.begin();
		}
	}
}