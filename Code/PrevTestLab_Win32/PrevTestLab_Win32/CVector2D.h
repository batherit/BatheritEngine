#pragma once
#include<iostream>
#include<limits>
#include<windows.h>
#include"CCommonUtil.h"

class CVector2D
{
public:
#define MIN_FLOAT 0.01f

	CVector2D() : x_(0.0f), y_(0.0f) {}
	CVector2D(float x, float y) : x_(x), y_(y) {}
	~CVector2D();

	void Zero(void) { x_ = 0.0f; y_ = 0.0f; }
	bool IsZero(void) const { return (x_*x_ + y_ * y_) < MIN_FLOAT; }

	inline float Length(void) const;
	inline float LengthSq(void) const;
	inline void Normalize(void);
	inline float Dot(const CVector2D& v2) const;
	inline int Sign(const CVector2D& v2) const;
	inline CVector2D Perp(void) const;
	inline void Truncate(float max);
	inline float Distance(const CVector2D &v2) const;
	inline float DistanceSq(const CVector2D& v2) const;
	inline void Reflect(const CVector2D& norm);
	inline CVector2D GetReverse() const;

	const CVector2D& operator+=(const CVector2D &rhs) {
		x_ += rhs.x_;
		y_ += rhs.y_;

		return *this;
	}

	const CVector2D& operator-=(const CVector2D &rhs) {
		x_ -= rhs.x_;
		y_ -= rhs.y_;

		return *this;
	}

	const CVector2D& operator*=(const float& rhs) {
		x_ *= rhs;
		y_ *= rhs;

		return *this;
	}

	const CVector2D& operator/=(const float& rhs) {
		x_ /= rhs;
		y_ /= rhs;

		return *this;
	}

	bool operator==(const CVector2D& rhs) const {
		return (IsEqual(x_, rhs.x_) && IsEqual(y_, rhs.y_));
	}

	bool operator!=(const CVector2D& rhs) const {
		return (x_ != rhs.x_) || (y_ != rhs.y_);
	}

	float x_;
	float y_;
};

inline CVector2D operator*(const CVector2D &lhs, float rhs);
inline CVector2D operator*(float lhs, const CVector2D &rhs);
inline CVector2D operator-(const CVector2D &lhs, const CVector2D &rhs);
inline CVector2D operator+(const CVector2D &lhs, const CVector2D &rhs);
inline CVector2D operator/(const CVector2D &lhs, float val);
std::ostream& operator<<(std::ostream& os, const CVector2D& rhs);
std::ifstream& operator>>(std::ifstream& is, CVector2D& lhs);

inline float CVector2D::Length() const {
	return sqrt(LengthSq());
}

inline float CVector2D::LengthSq() const {
	return (x_ * x_ + y_ * y_);
}

inline void CVector2D::Normalize() {
	float vector_length = this->Length();

	if (vector_length > std::numeric_limits<float>::epsilon())
	{
		this->x_ /= vector_length;
		this->y_ /= vector_length;
	}
}

inline CVector2D Vec2DNormalize(const CVector2D &v) {
	CVector2D vec = v;

	float vector_length = vec.Length();

	if (vector_length > std::numeric_limits<float>::epsilon())
	{
		vec.x_ /= vector_length;
		vec.y_ /= vector_length;
	}

	return vec;
}

inline float Vec2DDistance(const CVector2D &v1, const CVector2D &v2)
{

	float ySeparation = v2.y_ - v1.y_;
	float xSeparation = v2.x_ - v1.x_;

	return sqrt(ySeparation*ySeparation + xSeparation * xSeparation);
}

inline float CVector2D::Dot(const CVector2D& v2) const {
	return x_ * v2.x_ + y_ * v2.y_;
}

enum { clockwise = 1, anticlockwise = -1 };

inline int CVector2D::Sign(const CVector2D& v2)const
{
	if (y_*v2.x_ > x_*v2.y_)
	{
		return anticlockwise;
	}
	else
	{
		return clockwise;
	}
}

inline CVector2D CVector2D::Perp() const {
	return CVector2D(-y_, x_);
}

inline void CVector2D::Truncate(float max) {
	if (this->Length() > max)
	{
		this->Normalize();

		*this *= max;
	}
}

inline float CVector2D::Distance(const CVector2D &v2) const {
	float ySeparation = v2.y_ - y_;
	float xSeparation = v2.x_ - x_;

	return sqrt(ySeparation*ySeparation + xSeparation * xSeparation);
}

inline float CVector2D::DistanceSq(const CVector2D& v2) const {
	float y_separation = v2.y_ - y_;
	float x_separation = v2.x_ - x_;

	return y_separation * y_separation + x_separation * x_separation;
}

inline double Vec2DDistanceSq(const CVector2D &v1, const CVector2D &v2)
{

	float ySeparation = v2.y_ - v1.y_;
	float xSeparation = v2.x_ - v1.x_;

	return ySeparation * ySeparation + xSeparation * xSeparation;
}

inline void CVector2D::Reflect(const CVector2D& norm) {
	*this += 2.0 * this->Dot(norm) * norm.GetReverse();
}

inline CVector2D CVector2D::GetReverse() const {
	return CVector2D(-this->x_, -this->y_);
}

inline POINT VectorToPOINT(const CVector2D& v)
{
	POINT p;
	p.x = (long)v.x_;
	p.y = (long)v.y_;

	return p;
}

inline CVector2D operator*(const CVector2D &lhs, float rhs) {
	CVector2D result(lhs);
	result *= rhs;
	return result;
}

inline CVector2D operator*(float lhs, const CVector2D &rhs) {
	CVector2D result(rhs);
	result *= lhs;
	return result;
}

inline CVector2D operator-(const CVector2D &lhs, const CVector2D &rhs) {
	CVector2D result(lhs);
	result.x_ -= rhs.x_;
	result.y_ -= rhs.y_;

	return result;
}

inline CVector2D operator+(const CVector2D &lhs, const CVector2D &rhs) {
	CVector2D result(lhs);
	result.x_ += rhs.x_;
	result.y_ += rhs.y_;

	return result;
}

inline CVector2D operator/(const CVector2D &lhs, float val) {
	CVector2D result(lhs);
	result.x_ /= val;
	result.y_ /= val;

	return result;
}

inline void WrapAround(CVector2D &pos, int MaxX, int MaxY)
{
	if (pos.x_ > MaxX) { pos.x_ = 0.0; }

	if (pos.x_ < 0) { pos.x_ = (float)MaxX; }

	if (pos.y_ < 0) { pos.y_ = (float)MaxY; }

	if (pos.y_ > MaxY) { pos.y_ = 0.0; }
}