#pragma once

#include "ADXVector3.h"
#include "ADXMatrix4.h"
#include <d3d12.h>

// 定数バッファ用データ構造体
struct ConstBufferDataTransform {
	XMMATRIX matWorld;           // ローカル → ワールド変換行列
};

/// <summary>
/// ワールド変換データ
/// </summary>
struct ADXWorldTransform {
	// 定数バッファ
	ID3D12Resource* constBuffTransform = nullptr;
	// マッピング済みアドレス
	ConstBufferDataTransform* constMapTransform = nullptr;
	// ローカルスケール
	ADXVector3 scale_ = { 1, 1, 1 };
	// X,Y,Z軸回りのローカル回転角
	ADXVector3 rotation_ = { 0, 0, 0 };
	// ローカル座標
	ADXVector3 translation_ = { 0, 0, 0 };
	// ローカル → ワールド変換行列
	ADXMatrix4 matWorld_;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 行列を更新する
	/// </summary>
	void UpdateMatrix(XMMATRIX matView, XMMATRIX matProjection);
};
