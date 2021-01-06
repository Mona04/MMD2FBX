#include "Framework.h"
#include "Matrix.h"
#include "Math/Math.h"

// DirectX 에서 넣으면 transpose 되어서 들어간다. 나는 DirectX 내에선 column 우선, Hlsl 은 row 우선으로 할 생각이다.
const Matrix Matrix::identity;

const Matrix Matrix::HomogeneousToCartesian(const Matrix& matrix)
{
	return Matrix
	(
		matrix._11, matrix._12, matrix._13, 0,
		matrix._21, matrix._22, matrix._23, 0,
		matrix._31, matrix._32, matrix._33, 0,
		0, 0, 0, 1
	);
}

const Matrix Matrix::PositionToMatrix(const Vector3& pos)
{
	return Matrix(1, 0, 0, pos.x, 0, 1, 0, pos.y, 0, 0, 1, pos.z, 0, 0, 0, 1);
}

const Matrix Matrix::ScaleToMatrix(const Vector3& scale)
{
	return Matrix(scale.x, 0, 0, 0, 0, scale.y, 0, 0, 0, 0, scale.z, 0, 0, 0, 0, 1);
}

Matrix const Matrix::QuaternionToMatrix(const Quaternion& rotation)
{
	float x2 = rotation.x * rotation.x;
	float y2 = rotation.y * rotation.y;
	float z2 = rotation.z * rotation.z;
	float w2 = rotation.w * rotation.w;
	float xy = rotation.x * rotation.y;
	float zw = rotation.z * rotation.w;
	float xz = rotation.z * rotation.x;
	float yw = rotation.y * rotation.w;
	float yz = rotation.y * rotation.z;
	float xw = rotation.x * rotation.w;

	return Matrix
	(
		x2 - y2 - z2 + w2,  2.0f * (xy - zw),    2.0f * (xz + yw),    0.0f,  
		2.0f * (zw + xy),   -x2 + y2 - z2 + w2,  2.0f * (yz - xw),    0.0f,
		2.0f * (xz - yw),   2.0f * (xw + yz),    -x2 - y2 + z2 + w2,  0.0f,
		0.0f,     		    0.0f,       		 0.0f,                1.0f
	);
}

const Vector3 Matrix::MatrixToPosition(const Matrix& matrix)
{
	return Vector3(matrix._14, matrix._24, matrix._34);
}

const Vector3 Matrix::MatrixToScale(const Matrix& matrix)
{
	//int sign_x = Math::Sign(matrix._11 * matrix._12 * matrix._13) < 0 ? -1 : 1;   // 이거 없어서 털림
	//int sign_y = Math::Sign(matrix._21 * matrix._22 * matrix._23) < 0 ? -1 : 1;
	//int sign_z = Math::Sign(matrix._31 * matrix._32 * matrix._33) < 0 ? -1 : 1;  // 이건 2D 에서나 가능함

	return Vector3
	(
		sqrtf(powf(matrix._11, 2) + powf(matrix._12, 2) + powf(matrix._13, 2)),
		sqrtf(powf(matrix._21, 2) + powf(matrix._22, 2) + powf(matrix._23, 2)),
		sqrtf(powf(matrix._31, 2) + powf(matrix._32, 2) + powf(matrix._33, 2))
	);
}

const Quaternion Matrix::MatrixToRotation(const Matrix& matrix)
{
	Quaternion quaternion;
	Vector3 scale = MatrixToScale(matrix);

	float m11 = matrix._11 / scale.x;
	float m12 = matrix._12 / scale.y;
	float m13 = matrix._13 / scale.z;
	float m21 = matrix._21 / scale.x;
	float m22 = matrix._22 / scale.y;
	float m23 = matrix._23 / scale.z;
	float m31 = matrix._31 / scale.x;
	float m32 = matrix._32 / scale.y;
	float m33 = matrix._33 / scale.z;

	float sq;
	float tr = m11 + m22 + m33;

	if (tr > -1.0f)  // tr + 1 >= 0 => w 구함
	{
		sq = sqrt(tr + 1.0f);
		quaternion.w = sq * 0.5f;

		quaternion.x = (m32 - m23) / (4 * quaternion.w);
		quaternion.y = (m13 - m31) / (4 * quaternion.w);
		quaternion.z = (m21 - m12) / (4 * quaternion.w);

		return quaternion;
	}
	else if (m11 > m22 && m11 > m33)  // x 구함
	{
		sq = sqrt(1.0f + m11 - m22 - m33);
		quaternion.x = sq * 0.5f;

		quaternion.y = (m12 + m21) / (4 * quaternion.x);
		quaternion.z = (m13 + m31) / (4 * quaternion.x);
		quaternion.w = (m32 - m32) / (4 * quaternion.x);

		return quaternion;
	}
	if (m22 > m11 && m22 > m33)  // y 구함
	{
		sq = sqrt(1.0f + m22 - m11 - m33);
		quaternion.y = sq * 0.5f;

		quaternion.x = (m21 + m12) / (4 * quaternion.y);
		quaternion.z = (m32 + m23) / (4 * quaternion.y);
		quaternion.w = (m32 - m23) / (4 * quaternion.y);

		return quaternion;
	}
	else
	{
		sq = sqrt(1.0f + m33 - m11 - m22);   // z 구함
		quaternion.z = sq * 0.5f;

		quaternion.x = (m31 + m13) / (4 * quaternion.z);
		quaternion.y = (m32 + m23) / (4 * quaternion.z);
		quaternion.w = (m21 - m12) / (4 * quaternion.z);

		return quaternion;
	}
}


Matrix::Matrix()
{
	SetIdentity();
}

Matrix::Matrix(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44)
	: _11(_11), _12(_12), _13(_13), _14(_14)
	, _21(_21), _22(_22), _23(_23), _24(_24)
	, _31(_31), _32(_32), _33(_33), _34(_34)
	, _41(_41), _42(_42), _43(_43), _44(_44)
{}

void Matrix::SetIdentity()
{
	_11 = 1; _12 = 0; _13 = 0; _14 = 0;
	_21 = 0; _22 = 1; _23 = 0; _24 = 0;
	_31 = 0; _32 = 0; _33 = 1; _34 = 0;
	_41 = 0; _42 = 0; _43 = 0; _44 = 1;
}

Matrix Matrix::Transpose() const
{
	Matrix tmp = *this;
	tmp._11 = _11; tmp._12 = _21; tmp._13 = _31; tmp._14 = _41;
	tmp._21 = _12; tmp._22 = _22; tmp._23 = _32; tmp._24 = _42;
	tmp._31 = _13; tmp._32 = _23; tmp._33 = _33; tmp._34 = _43;
	tmp._41 = _14; tmp._42 = _24; tmp._43 = _34; tmp._44 = _44;
	return tmp;
}

Matrix Matrix::Inverse_SRT() const
{
	Matrix SR = Matrix::HomogeneousToCartesian(*this).Transpose();	
	Matrix T = Matrix::identity;
	T._14 = -this->_14;
	T._24 = -this->_24;
	T._34 = -this->_34;
	return SR * T;
}
//void Matrix::operator=(const Matrix& rhs)
//{
//	memcpy(this, &rhs, sizeof(Matrix));
//}

Matrix Matrix::operator*(float rhs) const
{
	Matrix result = *this;
	result *= rhs;

	return result;
}

Matrix& Matrix::operator*=(float rhs)
{
	_11 *= rhs; _22 *= rhs; _33 *= rhs; 

	return *this;
}

const Matrix Matrix::operator*(const Matrix& rhs) const
{
	return Matrix
	(
		_11 * rhs._11 + _12 * rhs._21 + _13 * rhs._31 + _14 * rhs._41,
		_11 * rhs._12 + _12 * rhs._22 + _13 * rhs._32 + _14 * rhs._42,
		_11 * rhs._13 + _12 * rhs._23 + _13 * rhs._33 + _14 * rhs._43,
		_11 * rhs._14 + _12 * rhs._24 + _13 * rhs._34 + _14 * rhs._44,
		_21 * rhs._11 + _22 * rhs._21 + _23 * rhs._31 + _24 * rhs._41,
		_21 * rhs._12 + _22 * rhs._22 + _23 * rhs._32 + _24 * rhs._42,
		_21 * rhs._13 + _22 * rhs._23 + _23 * rhs._33 + _24 * rhs._43,
		_21 * rhs._14 + _22 * rhs._24 + _23 * rhs._34 + _24 * rhs._44,
		_31 * rhs._11 + _32 * rhs._21 + _33 * rhs._31 + _34 * rhs._41,
		_31 * rhs._12 + _32 * rhs._22 + _33 * rhs._32 + _34 * rhs._42,
		_31 * rhs._13 + _32 * rhs._23 + _33 * rhs._33 + _34 * rhs._43,
		_31 * rhs._14 + _32 * rhs._24 + _33 * rhs._34 + _34 * rhs._44,
		_41 * rhs._11 + _42 * rhs._21 + _43 * rhs._31 + _44 * rhs._41,
		_41 * rhs._12 + _42 * rhs._22 + _43 * rhs._32 + _44 * rhs._42,
		_41 * rhs._13 + _42 * rhs._23 + _43 * rhs._33 + _44 * rhs._43,
		_41 * rhs._14 + _42 * rhs._24 + _43 * rhs._34 + _44 * rhs._44
	);
}

const Vector3 Matrix::operator*(const Vector3& rhs) const
{
	return Vector3(
		_11 *rhs.x + _12 * rhs.y + _13 * rhs.z + _14 * 1, 
		_21 * rhs.x + _22 * rhs.y + _23 * rhs.z + _24 * 1,
		_31 * rhs.x + _32 * rhs.y + _33 * rhs.z + _34 * 1
		);
}

const Vector4 Matrix::operator*(const Vector4& rhs) const
{
	return Vector4(
		_11 * rhs.x + _12 * rhs.y + _13 * rhs.z + _14 * rhs.w,
		_21 * rhs.x + _22 * rhs.y + _23 * rhs.z + _24 * rhs.w,
		_31 * rhs.x + _32 * rhs.y + _33 * rhs.z + _34 * rhs.w,
		_41 * rhs.x + _42 * rhs.y + _43 * rhs.z + _44 * rhs.w
	);
}

void Matrix::LookAtUpLH(const Vector3& eye, const Vector3& at, const Vector3& up)
{
	Vector3 zaxis = (at - eye).Normalize();
	Vector3 xaxis = Vector3::Cross(up, zaxis).Normalize();
	Vector3 yaxis = Vector3::Cross(zaxis, xaxis);

	_11 = xaxis.x;  _12 = xaxis.y;  _13 = xaxis.z;  _14 = -Vector3::Dot(xaxis, eye);
	_21 = yaxis.x;  _22 = yaxis.y;  _23 = yaxis.z;  _24 = -Vector3::Dot(yaxis, eye);
	_31 = zaxis.x;  _32 = zaxis.y;  _33 = zaxis.z;  _34 = -Vector3::Dot(zaxis, eye);
	_41 = 0.0f;     _42 = 0.0f;     _43 = 0.0f;     _44 = 1.0f;
}

void Matrix::PerspectiveFovLH(float fovAngleY, float aspectRatio, float zNear, float zFar)
{
	float h = 1 / std::tanf(fovAngleY * 0.5f);    //  z / h 이기 때문에 아래에서 w / h 를 나눠야지 z / w 가 됨. 
	float w = h / aspectRatio;   // 그리고 후면제거 단계 혹은 그 이전인 ps 이런데에서 w 에 저장되어 있는 z 값을 나누면   -1 ~ +1 로 처리가 됨.
	SetIdentity();

	_11 = w; 
			_22 = h; 
		     	     _33 = zFar / (zFar - zNear); _34 = -zNear * zFar / (zFar - zNear);
					 _43 = 1; _44 = 0;
}

void Matrix::OrthographicLH(float w, float h, float zNear, float zFar)
{
	SetIdentity();

	_11 = 2/w; 
			_22 = 2/h; 
		     	     _33 = 1 / (zFar - zNear); _34 = zNear / (zNear - zFar);
											   _44 = 1;
}