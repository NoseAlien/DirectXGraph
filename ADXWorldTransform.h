#pragma once

#include "ADXVector3.h"
#include "ADXMatrix4.h"

// 定数バッファ用データ構造体
struct ConstBufferDataWorldTransform {
	ADXMatrix4 matWorld;           // ローカル → ワールド変換行列
};

/// <summary>
/// ワールド変換データ
/// </summary>
struct ADXWorldTransform {
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
	void UpdateMatrix();
};
