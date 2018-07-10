#pragma once
#include<string>
#include<windows.h>

void ChangeMenuState(HWND hwnd, UINT MenuItem, UINT state);

LPCWSTR ConvertToUnicode(const char* s);
LPCWSTR ConvertToUnicode(const std::string &s);