#pragma once
#include<vector>
#include "CVector2D.h"

class C2DMatrix
{
public:
	C2DMatrix() {
		Identity();
	}

	//create an identity matrix
	inline void Identity();

	//create a transformation matrix
	inline void Translate(float x, float y);

	//create a scale matrix
	inline void Scale(float xScale, float yScale);

	//create a rotation matrix
	inline void  Rotate(float rotation);

	//create a rotation matrix from a fwd and side 2D vector
	inline void  Rotate(const CVector2D &fwd, const CVector2D &side);

	//applys a transformation matrix to a std::vector of points
	inline void TransformVector2Ds(std::vector<CVector2D> &vPoints);

	//applys a transformation matrix to a point
	inline void TransformVector2Ds(CVector2D &vPoint);

	//accessors to the matrix elements
	void _11(float val) { matrix_._11 = val; }
	void _12(float val) { matrix_._12 = val; }
	void _13(float val) { matrix_._13 = val; }

	void _21(float val) { matrix_._21 = val; }
	void _22(float val) { matrix_._22 = val; }
	void _23(float val) { matrix_._23 = val; }

	void _31(float val) { matrix_._31 = val; }
	void _32(float val) { matrix_._32 = val; }
	void _33(float val) { matrix_._33 = val; }

	~C2DMatrix();

private:
	struct Matrix {
		float _11, _12, _13;
		float _21, _22, _23;
		float _31, _32, _33;

		Matrix()
		{
			_11 = 0.0; _12 = 0.0; _13 = 0.0;
			_21 = 0.0; _22 = 0.0; _23 = 0.0;
			_31 = 0.0; _32 = 0.0; _33 = 0.0;
		}
	};

	Matrix matrix_;

	//multiplies m_Matrix with mIn
	inline void  MatrixMultiply(Matrix &mIn);
};

inline void C2DMatrix::MatrixMultiply(Matrix &mIn)
{
	C2DMatrix::Matrix mat_temp;

	//first row
	mat_temp._11 = (matrix_._11*mIn._11) + (matrix_._12*mIn._21) + (matrix_._13*mIn._31);
	mat_temp._12 = (matrix_._11*mIn._12) + (matrix_._12*mIn._22) + (matrix_._13*mIn._32);
	mat_temp._13 = (matrix_._11*mIn._13) + (matrix_._12*mIn._23) + (matrix_._13*mIn._33);

	//second
	mat_temp._21 = (matrix_._21*mIn._11) + (matrix_._22*mIn._21) + (matrix_._23*mIn._31);
	mat_temp._22 = (matrix_._21*mIn._12) + (matrix_._22*mIn._22) + (matrix_._23*mIn._32);
	mat_temp._23 = (matrix_._21*mIn._13) + (matrix_._22*mIn._23) + (matrix_._23*mIn._33);

	//third
	mat_temp._31 = (matrix_._31*mIn._11) + (matrix_._32*mIn._21) + (matrix_._33*mIn._31);
	mat_temp._32 = (matrix_._31*mIn._12) + (matrix_._32*mIn._22) + (matrix_._33*mIn._32);
	mat_temp._33 = (matrix_._31*mIn._13) + (matrix_._32*mIn._23) + (matrix_._33*mIn._33);

	matrix_ = mat_temp;
}

//applies a 2D transformation matrix to a std::vector of Vector2Ds
inline void C2DMatrix::TransformVector2Ds(std::vector<CVector2D> &vPoint)
{
	for (unsigned int i = 0; i<vPoint.size(); ++i)
	{
		float tempX = (matrix_._11*vPoint[i].x_) + (matrix_._21*vPoint[i].y_) + (matrix_._31);

		float tempY = (matrix_._12*vPoint[i].x_) + (matrix_._22*vPoint[i].y_) + (matrix_._32);

		vPoint[i].x_ = tempX;

		vPoint[i].y_ = tempY;

	}
}

//applies a 2D transformation matrix to a single Vector2D
inline void C2DMatrix::TransformVector2Ds(CVector2D &vPoint)
{

	float tempX = (matrix_._11*vPoint.x_) + (matrix_._21*vPoint.y_) + (matrix_._31);

	float tempY = (matrix_._12*vPoint.x_) + (matrix_._22*vPoint.y_) + (matrix_._32);

	vPoint.x_ = tempX;

	vPoint.y_ = tempY;
}



//create an identity matrix
inline void C2DMatrix::Identity()
{
	matrix_._11 = 1; matrix_._12 = 0; matrix_._13 = 0;

	matrix_._21 = 0; matrix_._22 = 1; matrix_._23 = 0;

	matrix_._31 = 0; matrix_._32 = 0; matrix_._33 = 1;

}

//create a transformation matrix
inline void C2DMatrix::Translate(float x, float y)
{
	Matrix mat;

	mat._11 = 1; mat._12 = 0; mat._13 = 0;

	mat._21 = 0; mat._22 = 1; mat._23 = 0;

	mat._31 = x;    mat._32 = y;    mat._33 = 1;

	//and multiply
	MatrixMultiply(mat);
}

//create a scale matrix
inline void C2DMatrix::Scale(float xScale, float yScale)
{
	C2DMatrix::Matrix mat;

	mat._11 = xScale; mat._12 = 0; mat._13 = 0;

	mat._21 = 0; mat._22 = yScale; mat._23 = 0;

	mat._31 = 0; mat._32 = 0; mat._33 = 1;

	//and multiply
	MatrixMultiply(mat);
}


//create a rotation matrix
inline void C2DMatrix::Rotate(float rot)
{
	C2DMatrix::Matrix mat;

	float Sin = sin(rot);
	float Cos = cos(rot);

	mat._11 = Cos;  mat._12 = Sin; mat._13 = 0;

	mat._21 = -Sin; mat._22 = Cos; mat._23 = 0;

	mat._31 = 0; mat._32 = 0; mat._33 = 1;

	//and multiply
	MatrixMultiply(mat);
}


//create a rotation matrix from a 2D vector
inline void C2DMatrix::Rotate(const CVector2D &fwd, const CVector2D &side)
{
	C2DMatrix::Matrix mat;

	mat._11 = fwd.x_;  mat._12 = fwd.y_; mat._13 = 0;

	mat._21 = side.x_; mat._22 = side.y_; mat._23 = 0;

	mat._31 = 0; mat._32 = 0; mat._33 = 1;

	//and multiply
	MatrixMultiply(mat);
}
