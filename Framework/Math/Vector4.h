#pragma once

class Vector4 final
{
public:
	Vector4();
	Vector4(float x);
	Vector4(float x, float y, float z, float w);
	Vector4(const class Vector3& var, float w);
	~Vector4() = default;

	bool operator==(const Vector4& rhs) const;
	bool operator!=(const Vector4& rhs) const;
	Vector4 operator+(const Vector4 rhs) const;
	Vector4 operator-(const Vector4 rhs) const;
	Vector4 operator*(float rhs) const;

	friend Vector4 operator-(const Vector4& rhs);

	float Length() const;

public:
	float x;
	float y;
	float z;
	float w;
};