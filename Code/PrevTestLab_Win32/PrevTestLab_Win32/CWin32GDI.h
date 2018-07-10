#pragma once
#include<Windows.h>
#include<string>
#include<vector>
#include<cassert>

#include"CWindowsUtil.h"
#include"CVector2D.h"

#define NUM_COLOR	15

const COLORREF g_colors[NUM_COLOR] = {
	RGB(255,0,0),
	RGB(0,0,255),
	RGB(0,255,0),
	RGB(0,0,0),
	RGB(255,200,200),
	RGB(200,200,200),
	RGB(255,255,0),
	RGB(255,170,0),
	RGB(255,0,170),
	RGB(133,90,0),
	RGB(255,255,255),
	RGB(0, 100, 0),        //dark green
	RGB(0, 255, 255),       //light blue
	RGB(200, 200, 200),     //light grey
	RGB(255, 230, 230)      //light pink
};


#define gdi CWin32GDI::Instance()

class CWin32GDI
{
public:
	
	//enumerate some colors
	enum
	{
		red,
		blue,
		green,
		black,
		pink,
		grey,
		yellow,
		orange,
		purple,
		brown,
		white,
		dark_green,
		light_blue,
		light_grey,
		light_pink,
		hollow
	};

	~CWin32GDI();

	int NumPenColors()const { return NUM_COLOR; }

	static CWin32GDI* Instance();

	void BlackPen() { if (hdc_) { SelectObject(hdc_, black_pen_); } }
	void WhitePen() { if (hdc_) { SelectObject(hdc_, white_pen_); } }
	void RedPen() { if (hdc_) { SelectObject(hdc_, red_pen_); } }
	void GreenPen() { if (hdc_) { SelectObject(hdc_, green_pen_); } }
	void BluePen() { if (hdc_) { SelectObject(hdc_, blue_pen_); } }
	void GreyPen() { if (hdc_) { SelectObject(hdc_, grey_pen_); } }
	void PinkPen() { if (hdc_) { SelectObject(hdc_, pink_pen_); } }
	void YellowPen() { if (hdc_) { SelectObject(hdc_, yellow_pen_); } }
	void OrangePen() { if (hdc_) { SelectObject(hdc_, orange_pen_); } }
	void PurplePen() { if (hdc_) { SelectObject(hdc_, purple_pen_); } }
	void BrownPen() { if (hdc_) { SelectObject(hdc_, brown_pen_); } }

	void DarkGreenPen() { if (hdc_) { SelectObject(hdc_, dark_green_pen_); } }
	void LightBluePen() { if (hdc_) { SelectObject(hdc_, light_blue_pen_); } }
	void LightGreyPen() { if (hdc_) { SelectObject(hdc_, light_grey_pen_); } }
	void LightPinkPen() { if (hdc_) { SelectObject(hdc_, light_pink_pen_); } }

	void ThickBlackPen() { if (hdc_) { SelectObject(hdc_, thick_black_pen_); } }
	void ThickWhitePen() { if (hdc_) { SelectObject(hdc_, thick_white_pen_); } }
	void ThickRedPen() { if (hdc_) { SelectObject(hdc_, thick_red_pen_); } }
	void ThickGreenPen() { if (hdc_) { SelectObject(hdc_, thick_green_pen_); } }
	void ThickBluePen() { if (hdc_) { SelectObject(hdc_, thick_blue_pen_); } }

	void BlackBrush() { if (hdc_)SelectObject(hdc_, GetStockObject(BLACK_BRUSH)); }
	void WhiteBrush() { if (hdc_)SelectObject(hdc_, GetStockObject(WHITE_BRUSH)); }
	void HollowBrush() { if (hdc_)SelectObject(hdc_, GetStockObject(HOLLOW_BRUSH)); }
	void GreenBrush() { if (hdc_)SelectObject(hdc_, green_brush_); }
	void RedBrush() { if (hdc_)SelectObject(hdc_, red_brush_); }
	void BlueBrush() { if (hdc_)SelectObject(hdc_, blue_brush_); }
	void GreyBrush() { if (hdc_)SelectObject(hdc_, grey_brush_); }
	void BrownBrush() { if (hdc_)SelectObject(hdc_, brown_brush_); }
	void YellowBrush() { if (hdc_)SelectObject(hdc_, yellow_brush_); }
	void LightBlueBrush() { if (hdc_)SelectObject(hdc_, light_blue_brush_); }
	void DarkGreenBrush() { if (hdc_)SelectObject(hdc_, dark_green_brush_); }
	void OrangeBrush() { if (hdc_)SelectObject(hdc_, orange_brush_); }

	//ALWAYS call this before drawing
	void StartDrawing(HDC hdc)
	{
		assert(hdc_ == NULL);

		hdc_ = hdc;

		//get the current pen
		old_pen_ = (HPEN)SelectObject(hdc, black_pen_);
		//select it back in
		SelectObject(hdc, old_pen_);

		old_brush_ = (HBRUSH)SelectObject(hdc, GetStockObject(BLACK_BRUSH));
		SelectObject(hdc, old_brush_);
	}

	//ALWAYS call this after drawing
	void StopDrawing(HDC hdc)
	{
		assert(hdc != NULL);

		SelectObject(hdc, old_pen_);
		SelectObject(hdc, old_brush_);

		hdc_ = NULL;
	}

	//---------------------------Text

	void TextAtPos(int x, int y, const std::string &s)
	{
		TextOut(hdc_, x, y, ConvertToUnicode(s), (int)s.size());
	}

	void TextAtPos(float x, float y, const std::string &s)
	{
		TextOut(hdc_, (int)x, (int)y, ConvertToUnicode(s), (int)s.size());
	}

	void TextAtPos(CVector2D pos, const std::string &s)
	{
		TextOut(hdc_, (int)pos.x_, (int)pos.y_, ConvertToUnicode(s), (int)s.size());
	}

	void TransparentText() { SetBkMode(hdc_, TRANSPARENT); }

	void OpaqueText() { SetBkMode(hdc_, OPAQUE); }

	void TextColor(int color) { assert(color < NUM_COLOR); SetTextColor(hdc_, g_colors[color]); }
	void TextColor(int r, int g, int b) { SetTextColor(hdc_, RGB(r, g, b)); }

	//----------------------------pixels
	void DrawDot(CVector2D pos, COLORREF color)
	{
		SetPixel(hdc_, (int)pos.x_, (int)pos.y_, color);
	}

	void DrawDot(int x, int y, COLORREF color)
	{
		SetPixel(hdc_, x, y, color);
	}

	//-------------------------Line Drawing

	void Line(CVector2D from, CVector2D to)
	{
		MoveToEx(hdc_, (int)from.x_, (int)from.y_, NULL);
		LineTo(hdc_, (int)to.x_, (int)to.y_);
	}

	void Line(int a, int b, int x, int y)
	{
		MoveToEx(hdc_, a, b, NULL);
		LineTo(hdc_, x, y);
	}

	void Line(float a, float b, float x, float y)
	{
		MoveToEx(hdc_, (int)a, (int)b, NULL);
		LineTo(hdc_, (int)x, (int)y);
	}



	void PolyLine(const std::vector<CVector2D>& points)
	{
		//make sure we have at least 2 points
		if (points.size() < 2) return;

		MoveToEx(hdc_, (int)points[0].x_, (int)points[0].y_, NULL);

		for (unsigned int p = 1; p<points.size(); ++p)
		{
			LineTo(hdc_, (int)points[p].x_, (int)points[p].y_);
		}
	}

	void LineWithArrow(CVector2D from, CVector2D to, float size)
	{
		CVector2D norm = Vec2DNormalize(to - from);

		//calculate where the arrow is attached
		CVector2D CrossingPoint = to - (norm * size);

		//calculate the two extra points required to make the arrowhead
		CVector2D ArrowPoint1 = CrossingPoint + (norm.Perp() * 0.4f * size);
		CVector2D ArrowPoint2 = CrossingPoint - (norm.Perp() * 0.4f * size);

		//draw the line
		MoveToEx(hdc_, (int)from.x_, (int)from.y_, NULL);
		LineTo(hdc_, (int)CrossingPoint.x_, (int)CrossingPoint.y_);

		//draw the arrowhead (filled with the currently selected brush)
		POINT p[3];

		p[0] = VectorToPOINT(ArrowPoint1);
		p[1] = VectorToPOINT(ArrowPoint2);
		p[2] = VectorToPOINT(to);

		SetPolyFillMode(hdc_, WINDING);
		Polygon(hdc_, p, 3);

	}

	void Cross(CVector2D pos, int diameter)
	{
		Line((int)pos.x_ - diameter, (int)pos.y_ - diameter, (int)pos.x_ + diameter, (int)pos.y_ + diameter);
		Line((int)pos.x_ - diameter, (int)pos.y_ + diameter, (int)pos.x_ + diameter, (int)pos.y_ - diameter);
	}


	//---------------------Geometry drawing methods

	void Rect(int left, int top, int right, int bot)
	{
		Rectangle(hdc_, left, top, right, bot);
	}

	void Rect(float left, float top, float right, float bot)
	{
		Rectangle(hdc_, (int)left, (int)top, (int)right, (int)bot);
	}



	void ClosedShape(const std::vector<CVector2D> &points)
	{
		MoveToEx(hdc_, (int)points[0].x_, (int)points[0].y_, NULL);

		for (unsigned int p = 1; p<points.size(); ++p)
		{
			LineTo(hdc_, (int)points[p].x_, (int)points[p].y_);
		}

		LineTo(hdc_, (int)points[0].x_, (int)points[0].y_);
	}


	void Circle(CVector2D pos, float radius)
	{
		Ellipse(hdc_,
			(int)(pos.x_ - radius),
			(int)(pos.y_ - radius),
			(int)(pos.x_ + radius + 1),
			(int)(pos.y_ + radius + 1));
	}

	void Circle(float x, float y, float radius)
	{
		Ellipse(hdc_,
			(int)(x - radius),
			(int)(y - radius),
			(int)(x + radius + 1),
			(int)(y + radius + 1));
	}

	void Circle(int x, int y, float radius)
	{
		Ellipse(hdc_,
			(int)(x - radius),
			(int)(y - radius),
			(int)(x + radius + 1),
			(int)(y + radius + 1));
	}


	void SetPenColor(int color)
	{
		assert(color < NUM_COLOR);

		switch (color)
		{
		case black:BlackPen(); return;

		case white:WhitePen(); return;
		case red: RedPen(); return;
		case green: GreenPen(); return;
		case blue: BluePen(); return;
		case pink: PinkPen(); return;
		case grey: GreyPen(); return;
		case yellow: YellowPen(); return;
		case orange: OrangePen(); return;
		case purple: PurplePen(); return;
		case brown: BrownPen(); return;
		case light_blue: LightBluePen(); return;
		case light_grey: LightGreyPen(); return;
		case light_pink: LightPinkPen(); return;
		}//end switch
	}

private:

	HPEN old_pen_;

	//all the pens
	HPEN   black_pen_;
	HPEN   white_pen_;
	HPEN   red_pen_;
	HPEN   green_pen_;
	HPEN   blue_pen_;
	HPEN   grey_pen_;
	HPEN   pink_pen_;
	HPEN   orange_pen_;
	HPEN   yellow_pen_;
	HPEN   purple_pen_;
	HPEN   brown_pen_;

	HPEN   dark_green_pen_;
	HPEN   light_blue_pen_;
	HPEN   light_grey_pen_;
	HPEN   light_pink_pen_;

	HPEN   thick_black_pen_;
	HPEN   thick_white_pen_;
	HPEN   thick_red_pen_;
	HPEN   thick_green_pen_;
	HPEN   thick_blue_pen_;

	HBRUSH old_brush_;

	//all the brushes
	HBRUSH  red_brush_;
	HBRUSH  green_brush_;
	HBRUSH  blue_brush_;
	HBRUSH  grey_brush_;
	HBRUSH  brown_brush_;
	HBRUSH  yellow_brush_;
	HBRUSH  orange_brush_;

	HBRUSH  light_blue_brush_;
	HBRUSH  dark_green_brush_;

	HDC    hdc_;

	//constructor is private
	CWin32GDI();

	//copy ctor and assignment should be private
	CWin32GDI(const CWin32GDI&);
	CWin32GDI& operator=(const CWin32GDI&);
};