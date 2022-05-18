#pragma once

#include <vector>
#include <DirectXMath.h>

using namespace DirectX;

class ADXModel
{
public:
	//���_�f�[�^�\����
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};

	std::vector<Vertex> vertices;
	std::vector <uint16_t> indices;
};