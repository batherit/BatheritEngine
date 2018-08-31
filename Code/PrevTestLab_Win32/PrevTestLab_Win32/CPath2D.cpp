#include "CPath2D.h"
#include "CMathUtil.h"
#include "CWin32GDI.h"
#include "CTransformComponent.h"


std::list<CVector2D> CPath2D::CreateRandomPath(
	int num_waypoints_,
	float min_X,
	float min_Y,
	float max_X,
	float max_Y) {
	waypoints_.clear();

	// 바운딩 박스의 중앙 좌표를 찾는다.
	float mid_X = (max_X + min_X) / 2.0f;
	float mid_Y = (max_Y + min_Y) / 2.0f;

	float smaller = min(mid_X, mid_Y);
	float spacing = TwoPi / (float)num_waypoints_;

	for (int i = 0; i < num_waypoints_; ++i) {
		float radial_dist = RandInRange(smaller*0.2f, smaller);

		CVector2D temp(radial_dist, 0.0f);

		Vec2DRotateAroundOrigin(temp, i*spacing);

		temp.x_ += mid_X; temp.y_ += mid_Y;
		
		waypoints_.push_back(temp);
	}

	cur_waypoint_ = waypoints_.begin();

	return waypoints_;
}

void CPath2D::Render()const
{
	gdi->OrangePen();

	std::list<CVector2D>::const_iterator it = waypoints_.begin();

	CVector2D wp = *it++;

	while (it != waypoints_.end())
	{
		gdi->Line(wp, *it);

		wp = *it++;
	}

	if (is_looped_) gdi->Line(*(--it), *waypoints_.begin());
}