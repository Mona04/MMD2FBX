#pragma once
#include "Vector3.h"
#include "Vector4.h"

// column 우선 matrix
class Matrix final
{
public:
	static const Matrix identity;

	// Homogeneous Coordinates(동차좌표계) -> Cartesian Coordinates(직교좌표계)
	static const Matrix HomogeneousToCartesian(const Matrix& matrix);

	static const Matrix PositionToMatrix(const Vector3& pos);
	static const Matrix ScaleToMatrix(const Vector3& scale);
	static const Matrix QuaternionToMatrix(const Quaternion& var);
	static const class Vector3 MatrixToPosition(const Matrix& matrix);
	static const class Vector3 MatrixToScale(const Matrix& matrix);
	static const class Quaternion MatrixToRotation(const Matrix& matrix);

public:
	Matrix();
	Matrix
	(
		float _11, float _12, float _13, float _14,
		float _21, float _22, float _23, float _24,
		float _31, float _32, float _33, float _34,
		float _41, float _42, float _43, float _44
	);
	~Matrix() = default;

public:
	void SetIdentity();
	Matrix Transpose() const;
	Matrix Inverse_SRT() const;

public:
	Matrix operator*(float rhs) const;
	Matrix& operator*=(float rhs);
	const Matrix operator*(const Matrix& rhs) const;
	const Vector3 operator*(const Vector3& rhs) const;
	const Vector4 operator*(const Vector4& rhs) const;

	void LookAtUpLH(const Vector3& eye, const Vector3& at, const Vector3& up);
	void PerspectiveFovLH(float fovAngleY, float aspectRatio, float zNear, float zFar);
	void OrthographicLH(float width, float height, float zNear, float zFar);

public:
	float _11, _12, _13, _14;
	float _21, _22, _23, _24;
	float _31, _32, _33, _34;
	float _41, _42, _43, _44;
};