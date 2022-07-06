#pragma once
#include "ADXModel.h"

class ADXObject
{
public:

	//定数バッファ用データ構造体（マテリアル）
	struct ConstBufferDataMaterial {
		XMFLOAT4 color;
	};

	ADXObject(HRESULT result, ID3D12Device* device);
	~ADXObject();
	void CreateConstBuffer(HRESULT result, ID3D12Device* device);

	ADXWorldTransform transform;
	ADXModel* model = nullptr;
};