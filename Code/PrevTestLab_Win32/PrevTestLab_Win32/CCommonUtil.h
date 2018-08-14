#pragma once
#include<cmath>
#include<cassert>

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

//returns a random integer between x and y
inline int   RandInt(int x, int y) {
	assert(y >= x && "<RandInt>: y is less than x");
	return rand() % (y - x + 1) + x;
}
inline float RandFloat() { return ((rand()) / (RAND_MAX + 1.0f)); }

//returns a random float in the range -1 < n < 1
inline float RandomClamped() { return RandFloat() - RandFloat(); }