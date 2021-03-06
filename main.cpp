#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#define DIRECTINPUT_VERSION 0x0800
#include "ADXKeyBoardInput.h"
#include "ADXModel.h"
#include "ADXWindow.h"
#include "ADXWorldTransform.h"
#include "ADXObject.h"
#include "ADXImage.h"
#include <DirectXTex.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

using namespace DirectX;


//定数バッファ用データ構造体（マテリアル）
struct ConstBufferDataMaterial {
	XMFLOAT4 color;
};

LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//WinAPI初期化処理
	const int window_width = 1280;
	const int window_height = 720;

	//ウィンドウ生成
	ADXWindow adxwindow(window_width, window_height,
		L"DirectXGame", (WNDPROC)windowProc);

	//メッセージ
	MSG msg{};

#ifdef _DEBUG
	//デバッグレイヤー有効化
	ID3D12Debug1* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

	//DX初期化
	HRESULT result;
	ID3D12Device* device = nullptr;
	IDXGIFactory7* dxgiFactory = nullptr;
	IDXGISwapChain4* swapChain = nullptr;
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12DescriptorHeap* rtvHeap = nullptr;

	//DirectInputの初期化
	IDirectInput8* directInput = nullptr;
	result = DirectInput8Create(
		adxwindow.w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//キーボードデバイスの生成
	ADXKeyBoardInput keyboard(result, GUID_SysKeyboard,
		&c_dfDIKeyboard, directInput,
		adxwindow.hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	//グラフィックボードのアダプタを列挙
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(result));

	std::vector<IDXGIAdapter4*> adapters;
	IDXGIAdapter4* tmpAdapter = nullptr;

	for (UINT i = 0;
		dxgiFactory->EnumAdapterByGpuPreference(i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
		i++)
	{
		adapters.push_back(tmpAdapter);
	}

	//アダプタの選別
	for (size_t i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC3 adapterDesk;
		adapters[i]->GetDesc3(&adapterDesk);

		if (!(adapterDesk.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			tmpAdapter = adapters[i];
			break;
		}
	}

	//対応レベルの配列
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	//デバイスの生成
	D3D_FEATURE_LEVEL featureLevel;

	for (size_t i = 0; i < _countof(levels); i++)
	{
		result = D3D12CreateDevice(tmpAdapter, levels[i],
			IID_PPV_ARGS(&device));
		if (result == S_OK)
		{
			featureLevel = levels[i];
			break;
		}
	}

	//コマンドアロケーターを生成
	result = device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator));
	assert(SUCCEEDED(result));

	//コマンドリストを生成
	result = device->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator, nullptr,
		IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(result));

	//コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//コマンドキューの生成
	result = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	assert(SUCCEEDED(result));

	//スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = 1280;
	swapChainDesc.Height = 720;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	//スワップチェーンの生成
	result = dxgiFactory->CreateSwapChainForHwnd(
		commandQueue, adxwindow.hwnd, &swapChainDesc, nullptr, nullptr,
		(IDXGISwapChain1**)&swapChain);
	assert(SUCCEEDED(result));

	//デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.NumDescriptors = swapChainDesc.BufferCount;
	//デスクリプタヒープの生成
	device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));

	//バックバッファ
	std::vector<ID3D12Resource*> backBuffers;
	backBuffers.resize(swapChainDesc.BufferCount);

	//スワップチェーンの全てのバッファについて処理する
	for (size_t i = 0; i < backBuffers.size(); i++)
	{
		swapChain->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers[i]));
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += i * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		//レンダーターゲットビューの生成
		device->CreateRenderTargetView(backBuffers[i], &rtvDesc, rtvHandle);
	}

	//フェンスの生成
	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;
	result = device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	//描画初期化処理
	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff);
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ID3D12Resource* constBuffMaterial = nullptr;
	//定数バッファの生成
	result = device->CreateCommittedResource(
		&cbHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(result));
	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);//マッピング
	assert(SUCCEEDED(result));
	//値を書き込むと自動的に転送される
	constMapMaterial->color = XMFLOAT4(1, 1, 1, 1);

	ID3D12Resource* constBuffTransform = nullptr;
	ConstBufferDataTransform* constMapTransform = nullptr;
	//この下のインデント部分は後で関数化する
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
			IID_PPV_ARGS(&constBuffTransform));
		assert(SUCCEEDED(result));
		//定数バッファのマッピング
		result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);//マッピング
		assert(SUCCEEDED(result));
	}

	constMapTransform->matWorld = XMMatrixIdentity();
	/*
	constMapTransform->mat.r[0].m128_f32[0] = 2.0f / window_width;
	constMapTransform->mat.r[1].m128_f32[1] = -2.0f / window_height;
	constMapTransform->mat.r[3].m128_f32[0] = -1.0f;
	constMapTransform->mat.r[3].m128_f32[1] = 1.0f;
	*/

	//平行投影行列の計算
	constMapTransform->matWorld = XMMatrixOrthographicOffCenterLH(
		0, window_width,
		window_height, 0,
		0.0f, 1.0f);
	//射影変換行列（透視投影）
	XMMATRIX matProjection =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(45.0f),//画角
			(float)window_width / window_height,//アスペクト比
			0.1f, 1000.0f);//ニア、ファークリップ

		//ルートパラメーターの設定
	D3D12_ROOT_PARAMETER rootParam = {};
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParam.Descriptor.ShaderRegister = 0;
	rootParam.Descriptor.RegisterSpace = 0;
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	//頂点シェーダーの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicVS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob, &errorBlob);

	//上の読み込みでエラーが起きたら出力ウィンドウに内容を表示
	if (FAILED(result))
	{
		//errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";

		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//ピクセルシェーダーの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicPS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);

	//上の読み込みでエラーが起きたら出力ウィンドウに内容を表示
	if (FAILED(result))
	{
		//errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";

		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{//三次元座標
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{//法線ベクトル
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{//uv座標
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}
	};

	//グラフィックスパイプラインの設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	//シェーダーの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	//サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//ラスタライザの設定
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; //背面をカリング
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineDesc.RasterizerState.DepthClipEnable = true;

	//ブレンドステート
	//pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	//頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	//図形の形状設定
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//その他の設定
	pipelineDesc.NumRenderTargets = 1;
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	pipelineDesc.SampleDesc.Count = 1;

	//リソース設定
	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = window_width; //レンダーターゲットに合わせる
	depthResourceDesc.Height = window_height; //レンダーターゲットに合わせる
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Format = DXGI_FORMAT_D32_FLOAT; //深度値フォーマット
	depthResourceDesc.SampleDesc.Count = 1;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; //デプスステンシル

	//深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f; //深度値1.0f（最大値）でクリア
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT; //深度値フォーマット

	//リソース生成
	ID3D12Resource* depthBuff = nullptr;
	result = device->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff));

	//深度ビュー用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	ID3D12DescriptorHeap* dsvHeap = nullptr;
	result = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	//深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(
		depthBuff,
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart());

	//デプスステンシルステートの設定
	pipelineDesc.DepthStencilState.DepthEnable = true; //深度テストを行う
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL; //書き込み許可
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS; //小さければ合格
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;


	//シェーダーリソースビューの最大個数
	const size_t kMaxSRVCount = 2056;
	//デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダーから見えるように
	srvHeapDesc.NumDescriptors = kMaxSRVCount;
	//設定を元にSRV用デスクリプタヒープを生成
	ID3D12DescriptorHeap* srvHeap = nullptr;
	result = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));

	//SRVヒープの先頭ハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();

	//デスクリプタのサイズ
	//ドライバによって違うので関数で取得しなければならない
	UINT incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


	//1枚目の画像
	ADXImage(L"Resources/napnose.png", srvHandle, device, result, incrementSize);

	//2枚目の画像
	ADXImage(L"Resources/MEGNOSE.png", srvHandle, device, result, incrementSize);

	//3枚目の画像
	ADXImage(L"Resources/whiteDot.png", srvHandle, device, result, incrementSize);


	//デスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;//一度の描画に使う画像が1枚なので
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;//テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメーターの設定
	D3D12_ROOT_PARAMETER rootParams[3] = {};
	//定数バッファ0番
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//種類
	rootParams[0].Descriptor.ShaderRegister = 0;//定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダーから見える
	//テクスチャレジスタ0番
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//種類
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;//デスクリプタレンジ
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;//デスクリプタレンジ数
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダーから見える
	//定数バッファ1番
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//種類
	rootParams[2].Descriptor.ShaderRegister = 1;//定数バッファ番号
	rootParams[2].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダーから見える

	//テクスチャサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//横繰り返し(タイリング)
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//縦繰り返し(タイリング)
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//奥行繰り返し(タイリング)
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;//全てリニア補完
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;//ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;//ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダーからのみ使用可能

	//ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;//ルートパラメーターの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams);//ルートパラメーター数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;
	//ルートシグネチャ
	ID3D12RootSignature* rootSignature;
	//ルートシグネチャのシリアライズ
	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();
	//パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature;
	// パイプランステートの生成
	ID3D12PipelineState* pipelineState = nullptr;
	result = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));


	ADXModel model;
	//頂点データ
	model.vertices = {
		//前
		{{-5.0f,-5.0f,-5.0f},{}, {0.0f,1.0f}},//左下
		{{-5.0f,5.0f,-5.0f},{},{0.0f,0.0f}},//左上
		{{5.0f,-5.0f,-5.0f},{},{1.0f,1.0f}},//右下
		{{5.0f,5.0f,-5.0f},{},{1.0f,0.0f}},//右上
		//後
		{{-5.0f,-5.0f,5.0f},{},{1.0f,1.0f}},//左下
		{{-5.0f,5.0f,5.0f},{},{1.0f,0.0f}},//左上
		{{5.0f,-5.0f,5.0f},{},{0.0f,1.0f}},//右下
		{{5.0f,5.0f,5.0f},{},{0.0f,0.0f}},//右上
		//左
		{{-5.0f,-5.0f,-5.0f},{},{1.0f,1.0f}},//左下
		{{-5.0f,-5.0f,5.0f},{},{0.0f,1.0f}},//左上
		{{-5.0f,5.0f,-5.0f},{},{1.0f,0.0f}},//右下
		{{-5.0f,5.0f,5.0f},{},{0.0f,0.0f}},//右上
		//右
		{{5.0f,-5.0f,-5.0f},{},{0.0f,1.0f}},//左下
		{{5.0f,-5.0f,5.0f},{},{1.0f,1.0f}},//左上
		{{5.0f,5.0f,-5.0f},{},{0.0f,0.0f}},//右下
		{{5.0f,5.0f,5.0f},{},{1.0f,0.0f}},//右上
		//下
		{{5.0f,-5.0f,-5.0f},{},{0.0f,1.0f}},//左下
		{{5.0f,-5.0f,5.0f},{},{0.0f,0.0f}},//左上
		{{-5.0f,-5.0f,-5.0f},{},{1.0f,1.0f}},//右下
		{{-5.0f,-5.0f,5.0f},{},{1.0f,0.0f}},//右上
		//上
		{{5.0f,5.0f,-5.0f},{},{1.0f,1.0f}},//左下
		{{5.0f,5.0f,5.0f},{},{1.0f,0.0f}},//左上
		{{-5.0f,5.0f,-5.0f},{},{0.0f,1.0f}},//右下
		{{-5.0f,5.0f,5.0f},{},{0.0f,0.0f}},//右上
	};
	//インデックスデータ
	model.indices =
	{
		//前
		0,1,2,
		2,1,3,
		//後
		5,4,6,
		5,6,7,
		//左
		8,9,10,
		10,9,11,
		//右
		13,12,14,
		13,14,15,
		//下
		16,17,18,
		18,17,19,
		//上
		21,20,22,
		21,22,23,
	};
	model.Initialize(device, result);

	ADXModel tri;
	tri.vertices = {
		{{-5.0f,-5.0f,0.0f},{}, {0.0f,1.0f}},//左下
		{{-5.0f,5.0f,0.0f},{},{0.0f,0.0f}},//左上
		{{5.0f,-5.0f,0.0f},{},{1.0f,1.0f}},//右下
	};
	//インデックスデータ
	tri.indices =
	{
		0,1,2,
	};
	tri.Initialize(device, result);

	ADXModel rect;
	//頂点データ
	rect.vertices = {
		{{-5.0f,-5.0f,0.0f},{}, {0.0f,1.0f}},//左下
		{{-5.0f,5.0f,0.0f},{},{0.0f,0.0f}},//左上
		{{5.0f,-5.0f,0.0f},{},{1.0f,1.0f}},//右下
		{{5.0f,5.0f,0.0f},{},{1.0f,0.0f}},//右上
	};
	//インデックスデータ
	rect.indices =
	{
		0,1,2,
		2,1,3,
	};
	rect.Initialize(device, result);

	//ビュー変換行列
	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, -100);
	XMFLOAT3 target(0, 0, 0);
	XMFLOAT3 up(0, 1, 0);
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	//ワールド変換行列

	ADXObject object_(result, device);
	object_.transform.translation_ = { -50,0,0 };
	object_.transform.rotation_ = { 0,0,0 };
	object_.transform.scale_ = { 1,1,1 };
	object_.transform.UpdateMatrix(matView, matProjection);

	ADXObject staticObject_(result, device);
	staticObject_.transform.translation_ = { 50,0,0 };
	staticObject_.transform.rotation_ = { 0,0,0 };
	staticObject_.transform.scale_ = { 1,1,1 };
	staticObject_.transform.UpdateMatrix(matView, matProjection);

	ADXObject triObject_(result, device);
	triObject_.transform.translation_ = { 0,0,0 };
	triObject_.transform.rotation_ = { 0,0,0 };
	triObject_.transform.scale_ = { 1,1,1 };
	triObject_.transform.UpdateMatrix(matView, matProjection);

	float angle = 0.0f;

	float hue = 0.0f;

	//キーボード情報の取得開始
	keyboard.keyboard->Acquire();

	//ゲームループ
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//xボタンが押されたらループを抜ける
		if (msg.message == WM_QUIT)
		{
			break;
		}

		keyboard.Update();

		//数字の0キーが押されていたら出力ウィンドウに「Hit 0」と表示
		if (keyboard.key[DIK_0])
		{
			OutputDebugStringA("Hit 0\n");
		}

		FLOAT clearColor[] = { 0.1f,0.25f,0.5f,0.0f };

		if (keyboard.key[DIK_Q] || keyboard.key[DIK_E])
		{
			if (keyboard.key[DIK_Q])
			{
				angle += XMConvertToRadians(0.5f);
			}
			if (keyboard.key[DIK_E])
			{
				angle -= XMConvertToRadians(0.5f);
			}

		}
		if (keyboard.key[DIK_W] || keyboard.key[DIK_S] || keyboard.key[DIK_D] || keyboard.key[DIK_A])
		{
			if (keyboard.key[DIK_W])
			{
				eye.z += 1;
			}
			else if (keyboard.key[DIK_S])
			{
				eye.z -= 1;
			}
			if (keyboard.key[DIK_D])
			{
				eye.x += 1;
			}
			else if (keyboard.key[DIK_A])
			{
				eye.x -= 1;
			}
		}
		target = { eye.x - 100 * sinf(angle), eye.y, eye.z + 100 * cosf(angle) };
		matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));


		if (keyboard.key[DIK_UP] || keyboard.key[DIK_DOWN] || keyboard.key[DIK_RIGHT] || keyboard.key[DIK_LEFT])
		{
			if (keyboard.key[DIK_UP])
			{
				object_.transform.translation_.y += 0.5;
			}
			if (keyboard.key[DIK_DOWN])
			{
				object_.transform.translation_.y -= 0.5;
			}
			if (keyboard.key[DIK_RIGHT])
			{
				object_.transform.translation_.x += 0.5;
			}
			if (keyboard.key[DIK_LEFT])
			{
				object_.transform.translation_.x -= 0.5;
			}
		}
		if (keyboard.key[DIK_I] || keyboard.key[DIK_O] || keyboard.key[DIK_P])
		{
			if (keyboard.key[DIK_I])
			{
				object_.transform.rotation_.x += 0.01;
			}
			if (keyboard.key[DIK_O])
			{
				object_.transform.rotation_.y += 0.01;
			}
			if (keyboard.key[DIK_P])
			{
				object_.transform.rotation_.z += 0.01;
			}
		}
		object_.transform.UpdateMatrix(matView, matProjection);

		staticObject_.transform.rotation_.z += 0.01f;
		staticObject_.transform.UpdateMatrix(matView, matProjection);

		triObject_.transform.UpdateMatrix(matView, matProjection);

		//DXの画面更新処理

		//バックバッファの番号取得
		UINT bbIndex = swapChain->GetCurrentBackBufferIndex();

		//リソースバリアで書き込み可能に変更
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = backBuffers[bbIndex];
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		commandList->ResourceBarrier(1, &barrierDesc);

		//描画先の変更
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
		commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

		//まず画面を背景色で塗り潰す
		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		//全ピクセルの深度バッファ値を最奥の1.0にする
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		//ここから描画コマンドを書き込む

		// ビューポート設定コマンド
		D3D12_VIEWPORT viewport{};
		viewport.Width = window_width;
		viewport.Height = window_height;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		// ビューポート設定コマンドを、コマンドリストに積む
		commandList->RSSetViewports(1, &viewport);

		// シザー矩形
		D3D12_RECT scissorRect{};
		scissorRect.left = 0; // 切り抜き座標左
		scissorRect.right = scissorRect.left + window_width; // 切り抜き座標右
		scissorRect.top = 0; // 切り抜き座標上
		scissorRect.bottom = scissorRect.top + window_height; // 切り抜き座標下
		// シザー矩形設定コマンドを、コマンドリストに積む
		commandList->RSSetScissorRects(1, &scissorRect);

		// パイプラインステートとルートシグネチャの設定コマンド
		commandList->SetPipelineState(pipelineState);
		commandList->SetGraphicsRootSignature(rootSignature);

		// プリミティブ形状の設定コマンド
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形リスト

		// 頂点バッファビューの設定コマンド
		commandList->IASetVertexBuffers(0, 1, &model.vbView);

		// インデックスバッファビューの設定コマンド
		commandList->IASetIndexBuffer(&model.ibView);

		//定数バッファビュー(CBV)の設定コマンド
		commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
		//SRVヒープの設定コマンド
		commandList->SetDescriptorHeaps(1, &srvHeap);
		//SRVヒープの先頭ハンドルを取得(SRVを指しているはず)
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
		//SRVヒープの先頭にあるSRVをルートパラメーター1番に設定
		commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

		hue += 0.5;
		if (hue < 60)
		{
			constMapMaterial->color = XMFLOAT4(1, hue / 60, 0, 1);
		}
		else if (hue < 120)
		{
			constMapMaterial->color = XMFLOAT4((120 - hue) / 60, 1, 0, 1);
		}
		else if (hue < 180)
		{
			constMapMaterial->color = XMFLOAT4(0, 1, (hue - 120) / 60, 1);
		}
		else if (hue < 240)
		{
			constMapMaterial->color = XMFLOAT4(0, (240 - hue) / 60, 1, 1);
		}
		else if (hue < 300)
		{
			constMapMaterial->color = XMFLOAT4((hue - 240) / 60, 0, 1, 1);
		}
		else if (hue < 360)
		{
			constMapMaterial->color = XMFLOAT4(1, 0, (360 - hue) / 60, 1);
		}
		else
		{
			hue = 0;
		}

		if (keyboard.key[DIK_SPACE])
		{
			constMapMaterial->color.w = 0.5;
		}

		//オブジェクトの描画
		model.Draw(commandList,object_.transform);

		srvGpuHandle.ptr += incrementSize;
		commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

		//オブジェクトの描画
		rect.Draw(commandList, staticObject_.transform);

		srvGpuHandle.ptr += incrementSize;
		commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

		//オブジェクトの描画
		tri.Draw(commandList, triObject_.transform);

		//ここまで描画コマンドを書き込む

		//リソースバッファを戻す
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		commandList->ResourceBarrier(1, &barrierDesc);

		//命令のクローズ
		result = commandList->Close();
		assert(SUCCEEDED(result));
		//コマンドリストの実行
		ID3D12CommandList* commandLists[] = { commandList };
		commandQueue->ExecuteCommandLists(1, commandLists);

		//画面に表示するバッファの表裏を入れ替える
		result = swapChain->Present(1, 0);
		assert(SUCCEEDED(result));

		//コマンドの実行完了を待機
		commandQueue->Signal(fence, ++fenceVal);
		if (fence->GetCompletedValue() != fenceVal)
		{
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			fence->SetEventOnCompletion(fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		//キューをクリア
		result = commandAllocator->Reset();
		assert(SUCCEEDED(result));
		//再度コマンドリストをためる準備
		result = commandList->Reset(commandAllocator, nullptr);
		assert(SUCCEEDED(result));
	}

	UnregisterClass(adxwindow.w.lpszClassName, adxwindow.w.hInstance);

	return 0;
}