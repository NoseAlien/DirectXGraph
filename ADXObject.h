#pragma once
#include "ADXWorldTransform.h"
#include "ADXModel.h"
#include <d3d12.h>

class ADXObject
{
public:
	ADXObject();
	~ADXObject();
	void CreateConstBuffer(HRESULT result, ID3D12Device* device);

	ID3D12Resource* constBuffTransform = nullptr;
	ConstBufferDataTransform* constMapTransform = nullptr;
	ADXWorldTransform transform;
	ADXModel* model = nullptr;
};