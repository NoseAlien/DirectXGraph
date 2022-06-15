#include "ADXVector3.h"
#include <cmath>

ADXVector3::ADXVector3()
{
	x = 0;
	y = 0;
	z = 0;
}

ADXVector3::ADXVector3(float x, float y, float z)
{
	ADXVector3::x = x;
	ADXVector3::y = y;
	ADXVector3::z = z;
}

float ADXVector3::length() const
{
	return sqrt(x * x + y * y + z * z);
}

ADXVector3& ADXVector3::normalize()
{
	float len = length();
	if (len != 0)
	{
		return *this /= len;
	}
	return *this;
}

float ADXVector3::dot(const ADXVector3& v) const
{
	return x * v.x + y * v.y + z * v.z;
}

ADXVector3 ADXVector3::cross(const ADXVector3& v) const
{
	return ADXVector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

ADXVector3 ADXVector3::operator+() const
{
	return *this;
}

ADXVector3 ADXVector3::operator-() const
{
	return ADXVector3(-x, -y, -z);
}

ADXVector3& ADXVector3::operator+=(const ADXVector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

ADXVector3& ADXVector3::operator-=(const ADXVector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

ADXVector3& ADXVector3::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

ADXVector3& ADXVector3::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

const ADXVector3 operator+(const ADXVector3& v1, const ADXVector3& v2)
{
	ADXVector3 temp(v1);
	return temp += v2;
}

const ADXVector3 operator-(const ADXVector3& v1, const ADXVector3& v2)
{
	ADXVector3 temp(v1);
	return temp -= v2;
}

const ADXVector3 operator*(const ADXVector3& v, float s)
{
	ADXVector3 temp(v);
	return temp *= s;
}

const ADXVector3 operator*(float s, const ADXVector3& v)
{
	ADXVector3 temp(v);
	return temp *= s;
}

const ADXVector3 operator/(const ADXVector3& v, float s)
{
	ADXVector3 temp(v);
	return temp /= s;
}