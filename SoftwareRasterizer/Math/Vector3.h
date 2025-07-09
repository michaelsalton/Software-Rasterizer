#pragma once

#include <cmath>

struct Vector3
{
	float x;
	float y;
	float z;

	Vector3() : x(0), y(0), z(0) {}
	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z){}

	float MagnitudeSquare()
	{
		return x * x + y * y + z * z;
	}

	float Magnitude()
	{
		return (float)sqrt(x * x + y * y + z * z);
	}

	Vector3 Normalize()
	{
		float magnitude = Magnitude();
		return Vector3(x / magnitude, y / magnitude, z / magnitude);
	}
};