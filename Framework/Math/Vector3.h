#pragma once

class Vector3 final
{
public:
	Vector3();
	Vector3(float x);
	Vector3(float x, float y, float z);
	~Vector3() = default;

	bool operator==(const Vector3& rhs) const;
	bool operator!=(const Vector3& rhs) const;
	Vector3 operator+(const Vector3 rhs) const;
	Vector3 operator-(const Vector3 rhs) const;
	Vector3 operator*(float rhs) const;
	Vector3 operator*(const Vector3 rhs) const;
	Vector3 operator*(const class Matrix& rhs) const;

	void operator*=(float rhs);

	friend Vector3 operator-(const Vector3& rhs);

	// right hand
	static Vector3 Cross(const Vector3& lhs, const Vector3& rhs);
	static float Dot(const Vector3& lhs, const Vector3& rhs);

	float Length() const;
	Vector3 Normalize() const;

	class Quaternion ToQuaternion() const;

public:
	float x;
	float y;
	float z;
};