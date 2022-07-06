#include "ADXObject.h"

ADXObject::ADXObject(HRESULT result, ID3D12Device* device)
{
	transform.Initialize();
	CreateConstBuffer(result, device);
}

ADXObject::~ADXObject()
{
	delete model;
}

void ADXObject::CreateConstBuffer(HRESULT result, ID3D12Device* device)
{
	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;//256バイトアラインメント
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//定数バッファの生成
	result = device->CreateCommittedResource(
		&cbHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&transform.constBuffTransform));
	assert(SUCCEEDED(result));
	//定数バッファのマッピング
	result = transform.constBuffTransform->Map(0, nullptr, (void**)&transform.constMapTransform);//マッピング
	assert(SUCCEEDED(result));
}