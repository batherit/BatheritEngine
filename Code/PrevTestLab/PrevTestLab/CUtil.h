#pragma once
#include<cmath>

class CUtil
{
public:
	CUtil();
	~CUtil();
};

inline bool IsEqual(float a, float b) {
	if (fabs(a - b) < 1E-12) {
		return true;
	}
	return false;
}