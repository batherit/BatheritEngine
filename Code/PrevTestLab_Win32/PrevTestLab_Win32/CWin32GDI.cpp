#include "CWin32GDI.h"

CWin32GDI* CWin32GDI::Instance() {
	static CWin32GDI instance;
	return &instance;
}

CWin32GDI::CWin32GDI()
{
	black_pen_ = CreatePen(PS_SOLID, 1, g_colors[black]);
	white_pen_ = CreatePen(PS_SOLID, 1, g_colors[white]);
	red_pen_ = CreatePen(PS_SOLID, 1, g_colors[red]);
	green_pen_ = CreatePen(PS_SOLID, 1, g_colors[green]);
	blue_pen_ = CreatePen(PS_SOLID, 1, g_colors[blue]);
	grey_pen_ = CreatePen(PS_SOLID, 1, g_colors[grey]);
	pink_pen_ = CreatePen(PS_SOLID, 1, g_colors[pink]);
	yellow_pen_ = CreatePen(PS_SOLID, 1, g_colors[yellow]);
	orange_pen_ = CreatePen(PS_SOLID, 1, g_colors[orange]);
	purple_pen_ = CreatePen(PS_SOLID, 1, g_colors[purple]);
	brown_pen_ = CreatePen(PS_SOLID, 1, g_colors[brown]);

	dark_green_pen_ = CreatePen(PS_SOLID, 1, g_colors[dark_green]);

	light_blue_pen_ = CreatePen(PS_SOLID, 1, g_colors[light_blue]);
	light_grey_pen_ = CreatePen(PS_SOLID, 1, g_colors[light_grey]);
	light_pink_pen_ = CreatePen(PS_SOLID, 1, g_colors[light_pink]);

	thick_black_pen_ = CreatePen(PS_SOLID, 2, g_colors[black]);
	thick_white_pen_ = CreatePen(PS_SOLID, 2, g_colors[white]);
	thick_red_pen_ = CreatePen(PS_SOLID, 2, g_colors[red]);
	thick_green_pen_ = CreatePen(PS_SOLID, 2, g_colors[green]);
	thick_blue_pen_ = CreatePen(PS_SOLID, 2, g_colors[blue]);

	green_brush_ = CreateSolidBrush(g_colors[green]);
	red_brush_ = CreateSolidBrush(g_colors[red]);
	blue_brush_ = CreateSolidBrush(g_colors[blue]);
	grey_brush_ = CreateSolidBrush(g_colors[grey]);
	brown_brush_ = CreateSolidBrush(g_colors[brown]);
	yellow_brush_ = CreateSolidBrush(g_colors[yellow]);
	light_blue_brush_ = CreateSolidBrush(RGB(0, 255, 255));
	dark_green_brush_ = CreateSolidBrush(g_colors[dark_green]);
	orange_brush_ = CreateSolidBrush(g_colors[orange]);

	hdc_ = NULL;
}


CWin32GDI::~CWin32GDI()
{
	DeleteObject(black_pen_);
	DeleteObject(white_pen_);
	DeleteObject(red_pen_);
	DeleteObject(green_pen_);
	DeleteObject(blue_pen_);
	DeleteObject(grey_pen_);
	DeleteObject(pink_pen_);
	DeleteObject(orange_pen_);
	DeleteObject(yellow_pen_);
	DeleteObject(purple_pen_);
	DeleteObject(brown_pen_);
	DeleteObject(old_pen_);

	DeleteObject(dark_green_pen_);

	DeleteObject(light_blue_pen_);
	DeleteObject(light_grey_pen_);
	DeleteObject(light_pink_pen_);

	DeleteObject(thick_black_pen_);
	DeleteObject(thick_white_pen_);
	DeleteObject(thick_red_pen_);
	DeleteObject(thick_green_pen_);
	DeleteObject(thick_blue_pen_);

	DeleteObject(green_brush_);
	DeleteObject(red_brush_);
	DeleteObject(blue_brush_);
	DeleteObject(old_brush_);
	DeleteObject(grey_brush_);
	DeleteObject(brown_brush_);
	DeleteObject(light_blue_brush_);
	DeleteObject(yellow_brush_);
	DeleteObject(dark_green_brush_);
	DeleteObject(orange_brush_);
}

