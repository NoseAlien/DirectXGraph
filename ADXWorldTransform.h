#pragma once

#include "ADXVector3.h"
#include "ADXMatrix4.h"
#include <d3d12.h>

// �萔�o�b�t�@�p�f�[�^�\����
struct ConstBufferDataTransform {
	XMMATRIX matWorld;           // ���[�J�� �� ���[���h�ϊ��s��
};

/// <summary>
/// ���[���h�ϊ��f�[�^
/// </summary>
struct ADXWorldTransform {
	// �萔�o�b�t�@
	ID3D12Resource* constBuffTransform = nullptr;
	// �}�b�s���O�ς݃A�h���X
	ConstBufferDataTransform* constMapTransform = nullptr;
	// ���[�J���X�P�[��
	ADXVector3 scale_ = { 1, 1, 1 };
	// X,Y,Z�����̃��[�J����]�p
	ADXVector3 rotation_ = { 0, 0, 0 };
	// ���[�J�����W
	ADXVector3 translation_ = { 0, 0, 0 };
	// ���[�J�� �� ���[���h�ϊ��s��
	ADXMatrix4 matWorld_;

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();
	/// <summary>
	/// �s����X�V����
	/// </summary>
	void UpdateMatrix(XMMATRIX matView, XMMATRIX matProjection);
};
