#pragma once

#include <DirectXMath.h>

using namespace DirectX;

/// <summary>
/// �s��
/// </summary>
class ADXMatrix4 {
public:
	// �sx��
	float m[4][4];

	// �R���X�g���N�^
	ADXMatrix4();
	// �������w�肵�Ă̐���
	ADXMatrix4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33);

	XMMATRIX ConvertToXMMatrix();

	//�P�ʍs��
	ADXMatrix4 IdentityMatrix();

	// ������Z�q�I�[�o�[���[�h
	ADXMatrix4& operator*=(const ADXMatrix4& m2);
};

const ADXMatrix4 operator*(const ADXMatrix4& m1, const ADXMatrix4& m2);
