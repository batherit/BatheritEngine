#pragma once
#include<limits>
#include"CVector2D.h"
#include"C2DMatrix.h"

const int     MaxInt = (std::numeric_limits<int>::max)();
const double  MaxDouble = (std::numeric_limits<double>::max)();
const double  MinDouble = (std::numeric_limits<double>::min)();
const float   MaxFloat = (std::numeric_limits<float>::max)();
const float   MinFloat = (std::numeric_limits<float>::min)();

const float   Pi = 3.14159f;
const float   TwoPi = Pi * 2;
const float   HalfPi = Pi / 2;
const float   QuarterPi = Pi / 4;

class CMathUtil
{
public:
	CMathUtil();
	~CMathUtil();
};

