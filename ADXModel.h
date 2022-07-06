#pragma once

#include <vector>
#include <d3d12.h>
#include "ADXWorldTransform.h"

using namespace DirectX;

class ADXModel
{
public:
	//頂点データ構造体
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 uv;
	};

	std::vector<Vertex> vertices;
	std::vector <uint16_t> indices;

	void SetNormal();
	void Draw(ID3D12GraphicsCommandList* commandList, ADXWorldTransform wtf_);
};