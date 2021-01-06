#include "Framework.h"
#include "Color4.h"

Color4::Color4() : r(0.0f), g(0.0f), b(0.0f), a(0.0f)
{
}

Color4::Color4(float var) : r(var), g(var), b(var), a(var)
{
}

Color4::Color4(float r, float g, float b, float a) : r(r), g(g), b(b), a(a)
{
}

//Color4::Color4(const Color4& in) : r(in.r), g(in.g), b(in.b), a(in.a)
//{
//}

Color4::Color4(const Vector3& vec) : r(vec.x), g(vec.y), b(vec.z), a(1)
{
}

Color4 Color4::operator*(float rhs) const
{
	return Color4(r*rhs, g*rhs, b*rhs, a*rhs);
}

Color4 Color4::Encode(unsigned int code)
{
	return Color4(float((code >> 0) & 0xff),
		float((code >> 8) & 0xff),
		float((code >> 16) & 0xff),
		float((code >> 24) & 0xff));
}

unsigned int Color4::Decode(Color4 color)
{
	return uint(((uint)(color.r) << 0 ) | 
				((uint)(color.g) << 8 ) |
				((uint)(color.b) << 16) |
				((uint)(color.a) << 24) );
}
