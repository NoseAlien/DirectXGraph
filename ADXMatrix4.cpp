#include "ADXMatrix4.h"

ADXMatrix4::ADXMatrix4()
{
	IdentityMatrix();
}

ADXMatrix4::ADXMatrix4(
	float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33)
{
	m[0][0] = m00;
	m[0][1] = m01;
	m[0][2] = m02;
	m[0][3] = m03;
	m[1][0] = m10;
	m[1][1] = m11;
	m[1][2] = m12;
	m[1][3] = m13;
	m[2][0] = m20;
	m[2][1] = m21;
	m[2][2] = m22;
	m[2][3] = m23;
	m[3][0] = m30;
	m[3][1] = m31;
	m[3][2] = m32;
	m[3][3] = m33;
}

ADXMatrix4 ADXMatrix4::IdentityMatrix()
{
	return
	{ 1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1 };
}

XMMATRIX ADXMatrix4::ConvertToXMMatrix()
{
	XMMATRIX outPutXMM;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			outPutXMM.r[i].m128_f32[j] = m[i][j];
		}
	}
	return outPutXMM;
}

ADXMatrix4& ADXMatrix4::operator*=(const ADXMatrix4& m2)
{
	ADXMatrix4 m1;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m1.m[i][j] = m[i][j];
		}
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j]
				+ m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
		}
	}
	return *this;
}

const ADXMatrix4 operator*(const ADXMatrix4& m1, const ADXMatrix4& m2)
{
	ADXMatrix4 temp(m1);
	return temp *= m2;
}