#include "CWindowsUtil.h"

void ChangeMenuState(HWND hwnd, UINT MenuItem, UINT state)
{
	MENUITEMINFO mi;

	mi.cbSize = sizeof(MENUITEMINFO);
	mi.fMask = MIIM_STATE;
	mi.fState = state;

	SetMenuItemInfo(GetMenu(hwnd), MenuItem, false, &mi);
	DrawMenuBar(hwnd);
}