#pragma once
#include "CWin32GDI.h"
#include "CVector2D.h"
#include <fstream>

class CWall2D
{
public:
	CWall2D();
	CWall2D(CVector2D A, CVector2D B) : v_A_(A), v_B_(B) {
		CalculateNormal();
	}
	CWall2D(CVector2D A, CVector2D B, CVector2D N) : v_A_(A), v_B_(B), v_N_(N){
	}
	CWall2D(std::ifstream& in) { Read(in); }

	virtual void Render(bool RenderNormals = false)const
	{
		gdi->Line(v_A_, v_B_);

		//render the normals if rqd
		if (RenderNormals)
		{
			int MidX = (int)((v_A_.x_ + v_B_.x_) / 2);
			int MidY = (int)((v_A_.y_ + v_B_.y_) / 2);

			gdi->Line(MidX, MidY, (int)(MidX + (v_N_.x_ * 5)), (int)(MidY + (v_N_.y_ * 5)));
		}
	}

	~CWall2D();

	CVector2D From()const { return v_A_; }
	void     SetFrom(CVector2D v) { v_A_ = v; CalculateNormal(); }

	CVector2D To()const { return v_B_; }
	void     SetTo(CVector2D v) { v_B_ = v; CalculateNormal(); }

	CVector2D Normal()const { return v_N_; }
	void     SetNormal(CVector2D n) { v_N_ = n; }

	CVector2D Center()const { return (v_A_ + v_B_) / 2.0; }

	std::ostream& Write(std::ostream& os)const
	{
		os << std::endl;
		os << From() << ",";
		os << To() << ",";
		os << Normal();
		return os;
	}

	void Read(std::ifstream& in) {
		float x, y;

		in >> x >> y;
		SetFrom(CVector2D(x, y));

		in >> x >> y;
		SetTo(CVector2D(x, y));

		in >> x >> y;
		SetNormal(CVector2D(x, y));
	}

protected:
	void CalculateNormal() {
		CVector2D temp = Vec2DNormalize(v_B_ - v_A_);

		v_N_.x_ = -temp.y_;
		v_N_.y_ = temp.x_;
	}

	CVector2D v_A_, v_B_, v_N_;
};

