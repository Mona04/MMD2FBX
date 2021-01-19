#pragma once
#include <cstdint>

class UInt8Vector2 final
{
public:
	UInt8Vector2() : x(0), y(0) {}
	UInt8Vector2(float x) : x(x), y(x) {}
	UInt8Vector2(float x, float y) : x(x), y(y) { }
	UInt8Vector2(const UInt8Vector2& vec) : x(vec.x), y(vec.y) {}
	~UInt8Vector2() = default;

public:
	uint8_t x;
	uint8_t y;
};