#pragma once
#include<cmath>

class CCommonUtil
{
public:
	CCommonUtil();
	~CCommonUtil();
};

inline bool IsEqual(float a, float b) {
	if (fabs(a - b) < 1E-12) {
		return true;
	}
	return false;
}

#define DEFINE_RAND_SEED srand(time(nullptr));

inline float RandFloat() { return ((rand()) / (RAND_MAX + 1.0f)); }

//returns a random float in the range -1 < n < 1
inline float RandomClamped() { return RandFloat() - RandFloat(); }