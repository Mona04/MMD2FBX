#pragma once

class Vector2 final
{
public:
	Vector2();
	Vector2(float x);
	Vector2(float x, float y);
	~Vector2() = default;

	bool operator!=(const Vector2& rhs) const;
	bool operator==(const Vector2& rhs) const;
	Vector2 operator+(const Vector2 rhs) const;
	Vector2 operator-(const Vector2 rhs) const;
	Vector2 operator-() const;
	Vector2 operator*(float rhs) const;
	Vector2 operator*(const Vector2& rhs) const;

	static float Dot(const Vector2& lhs, const Vector2& rhs);

	float Length() const;
	Vector2 Normalize() const;

public:
	float x;
	float y;
};