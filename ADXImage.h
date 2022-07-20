#pragma once
#include <DirectXTex.h>
#include <d3d12.h>
#include <cassert>

using namespace DirectX;

class ADXImage
{
public:
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	ScratchImage mipChain{};
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	D3D12_RESOURCE_DESC textureResourceDesc{};
	ID3D12Resource* texBuff = nullptr;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//ê›íËç\ë¢ëÃ

	ADXImage(const wchar_t* imgName, D3D12_CPU_DESCRIPTOR_HANDLE &srvHandle,ID3D12Device *device, HRESULT &result,UINT incrementSize);
};