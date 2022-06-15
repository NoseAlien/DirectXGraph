#pragma once

class ADXVector3
{
public:
	float x;
	float y;
	float z;

public:
	ADXVector3();
	ADXVector3(float x,float y,float z);

	float length() const;
	ADXVector3& normalize();
	float dot(const ADXVector3& v) const;
	ADXVector3 cross(const ADXVector3& v) const;

	ADXVector3 operator+() const;
	ADXVector3 operator-() const;

	ADXVector3& operator+=(const ADXVector3& v);
	ADXVector3& operator-=(const ADXVector3& v);
	ADXVector3& operator*=(float s);
	ADXVector3& operator/=(float s);
};

const ADXVector3 operator+(const ADXVector3& v1, const ADXVector3& v2);
const ADXVector3 operator-(const ADXVector3& v1, const ADXVector3& v2);
const ADXVector3 operator*(const ADXVector3& v, float s);
const ADXVector3 operator*(float s, const ADXVector3& v);
const ADXVector3 operator/(const ADXVector3& v, float s);
