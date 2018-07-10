#include "CWindowsUtil.h"
#include <windows.h>
#include <string>

void ChangeMenuState(HWND hwnd, UINT MenuItem, UINT state)
{
	MENUITEMINFO mi;

	mi.cbSize = sizeof(MENUITEMINFO);
	mi.fMask = MIIM_STATE;
	mi.fState = state;

	SetMenuItemInfo(GetMenu(hwnd), MenuItem, false, &mi);
	DrawMenuBar(hwnd);
}

LPCWSTR ConvertToUnicode(const char* s) {
	wchar_t strUnicode[256] = { 0, };
	char   strMultibyte[256] = { 0, };
	strcpy_s(strMultibyte, 256, s);
	int nLen = MultiByteToWideChar(CP_ACP, 0, strMultibyte, strlen(strMultibyte), NULL, NULL);
	MultiByteToWideChar(CP_ACP, 0, strMultibyte, strlen(strMultibyte), strUnicode, nLen);

	return strUnicode;
}

LPCWSTR ConvertToUnicode(const std::string &s) {
	int str_len = MultiByteToWideChar(CP_ACP, 0, &s[0], s.size(), NULL, NULL); 
	std::wstring strUni(str_len, 0); MultiByteToWideChar(CP_ACP, 0, &s[0], s.size(), &strUni[0], str_len);

	return strUni.c_str();
}