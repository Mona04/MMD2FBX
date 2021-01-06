#pragma once
#include "Vector3.h"

class Color4 final
{
public:
	Color4();
	Color4(float var);
	Color4(float r, float g, float b, float a);
	Color4(const Vector3& vec);
	//Color4(Color4& rhs);
	~Color4() = default;

	Color4 operator*(float rhs) const;

	static Color4 Encode(unsigned int code);
	static unsigned int Decode(Color4 code);

public:
	float r;
	float g;
	float b;
	float a;
};