#pragma once
#include<list>
#include<cassert>
#include "CVector2D.h"

class CPath2D
{
public:
	CPath2D() : is_looped_(false) {}

	// ������ ���� ����Ʈ���� ������ ��θ� ������ ������.
	// MinX/Y �׸��� MaxX/Y�� ����� �ٿ�� �ڽ��� �����Ѵ�.
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

	// ����Ʈ�� ���� �����ߴٸ� true�� ��ȯ�Ѵ�.
	bool Finished() { return !(cur_waypoint_ != waypoints_.end()); }

	// ����Ʈ���� ���� ��������Ʈ�� ���ͷ����͸� �̵���Ų��.
	inline void SetNextWaypoint();

	// min/max ������ ���ǵ� ���簢���� �ѷ����� ������ ��θ� ������.
	std::list<CVector2D> CreateRandomPath(
		int num_waypoints_,
		float min_X,
		float min_Y,
		float max_X,
		float max_Y);

	void LoopOn() { is_looped_ = true; }
	void LoopOff() { is_looped_ = false; }

	// ����� ���� ��������Ʈ�� �߰��Ѵ�.
	void AddWaypoint(CVector2D new_point);

	// �ٸ� ��γ� ���� ����Ʈ�� ���� ��θ� �����ϴ� �޼���
	void Set(std::list<CVector2D> new_path) { waypoints_ = new_path; cur_waypoint_ = waypoints_.begin(); }
	void Set(const CPath2D& path) { waypoints_ = path.GetPath(); cur_waypoint_ = waypoints_.begin(); }

	void Clear() { waypoints_.clear(); }
	std::list<CVector2D> GetPath() const { return waypoints_; }

	// ��θ� �������Ѵ�.
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