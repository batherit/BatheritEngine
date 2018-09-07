#pragma once
//-----------------------------------------------------------------------------
//
//  Name:   InvertedAABBox2D.h
//
//  Author: Mat Buckland (www.ai-junkie.com)
//
//	Desc:	v �ܼ� ������(��ũ�� �Ʒ������� y�� ����) �� ���� �ٿ�� �ڽ�
//			Ŭ����
//-----------------------------------------------------------------------------

#include "CVector2D.h"
#include "CWin32GDI.h"

class CInvertedAABBox2D
{
public:
	CInvertedAABBox2D(CVector2D tl,
		CVector2D br) :top_left_(tl),
		bottom_right_(br),
		center_((tl + br) / 2.0f)
	{}

	// �ٸ� ��ƼƼ�� ���ǵ� bbox�� �ٸ� ��ƼƼ�� �װͰ� �����ȴٸ� true�� ��ȯ�մϴ�. 
	bool IsOverlappedWith(const CInvertedAABBox2D& other)const
	{
		return !((other.Top() > this->Bottom()) ||
			(other.Bottom() < this->Top()) ||
			(other.Left() > this->Right()) ||
			(other.Right() < this->Left()));
	}


	CVector2D TopLeft()const { return top_left_; }
	CVector2D BottomRight()const { return bottom_right_; }

	float    Top()const { return top_left_.y_; }
	float    Left()const { return top_left_.x_; }
	float    Bottom()const { return bottom_right_.y_; }
	float    Right()const { return bottom_right_.x_; }
	CVector2D Center()const { return center_; }

	void     Render(bool RenderCenter = false)const
	{
		gdi->Line((int)Left(), (int)Top(), (int)Right(), (int)Top());
		gdi->Line((int)Left(), (int)Bottom(), (int)Right(), (int)Bottom());
		gdi->Line((int)Left(), (int)Top(), (int)Left(), (int)Bottom());
		gdi->Line((int)Right(), (int)Top(), (int)Right(), (int)Bottom());


		if (RenderCenter)
		{
			gdi->Circle(center_, 5);
		}
	}

private:
	CVector2D  top_left_;
	CVector2D  bottom_right_;
	CVector2D  center_;
};
