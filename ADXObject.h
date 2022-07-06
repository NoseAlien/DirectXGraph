#pragma once
#include "ADXModel.h"

class ADXObject
{
public:

	//�萔�o�b�t�@�p�f�[�^�\���́i�}�e���A���j
	struct ConstBufferDataMaterial {
		XMFLOAT4 color;
	};

	ADXObject(HRESULT result, ID3D12Device* device);
	~ADXObject();
	void CreateConstBuffer(HRESULT result, ID3D12Device* device);

	ADXWorldTransform transform;
	ADXModel* model = nullptr;
};