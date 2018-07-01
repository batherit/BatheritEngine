#include "CVector2D.h"
#include <fstream>

CVector2D::~CVector2D()
{
}

std::ostream& operator<<(std::ostream& os, const CVector2D& rhs)
{
	os << " " << rhs.x_ << " " << rhs.y_;

	return os;
}


std::ifstream& operator>>(std::ifstream& is, CVector2D& lhs)
{
	is >> lhs.x_ >> lhs.y_;

	return is;
}