#pragma once

#include <DirectXMath.h>

using namespace DirectX;

/// <summary>
/// 行列
/// </summary>
class ADXMatrix4 {
public:
	// 行x列
	float m[4][4];

	// コンストラクタ
	ADXMatrix4();
	// 成分を指定しての生成
	ADXMatrix4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33);

	XMMATRIX ConvertToXMMatrix();

	//単位行列
	ADXMatrix4 IdentityMatrix();

	// 代入演算子オーバーロード
	ADXMatrix4& operator*=(const ADXMatrix4& m2);
};

const ADXMatrix4 operator*(const ADXMatrix4& m1, const ADXMatrix4& m2);
