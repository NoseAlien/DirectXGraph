#include "ADXWorldTransform.h"
#include <math.h>

void ADXWorldTransform::Initialize()
{
	matWorld_ = matWorld_.IdentityMatrix();

}

void ADXWorldTransform::UpdateMatrix(XMMATRIX matView, XMMATRIX matProjection)
{
	//////拡縮//////
	//スケーリング倍率を行列に設定する
	ADXMatrix4 matScale =
	{ scale_.x,0,0,0,
	0,scale_.y,0,0,
	0,0,scale_.z,0,
	0,0,0,1 };

	//////回転//////
	//Z軸回転行列を宣言
	ADXMatrix4 matRotZ =
	{ (float)cos(rotation_.z),(float)sin(rotation_.z),0,0,
	(float)-sin(rotation_.z),(float)cos(rotation_.z),0,0,
	0,0,1,0,
	0,0,0,1 };
	//X軸回転行列を宣言
	ADXMatrix4 matRotX =
	{ 1,0,0,0,
	0,(float)cos(rotation_.x),(float)sin(rotation_.x),0,
	0,(float)-sin(rotation_.x),(float)cos(rotation_.x),0,
	0,0,0,1 };
	//Y軸回転行列を宣言
	ADXMatrix4 matRotY =
	{ (float)cos(rotation_.y),0,(float)-sin(rotation_.y),0,
	0,1,0,0,
	(float)sin(rotation_.y),0,(float)cos(rotation_.y),0,
	0,0,0,1 };

	//合成用回転行列を宣言し、ZXYの順に合成
	ADXMatrix4 matRot = matRotZ;
	matRot *= matRotX;
	matRot *= matRotY;

	//////平行移動//////
	//移動量を行列に設定する
	ADXMatrix4 matTrans =
	{ 1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	translation_.x,translation_.y,translation_.z,1 };


	//単位行列を代入
	matWorld_ = matWorld_.IdentityMatrix();
	//スケーリング行列を掛ける
	matWorld_ *= matScale;
	//合成用回転行列を掛ける
	matWorld_ *= matRot;
	//平行移動行列を掛ける
	matWorld_ *= matTrans;

	matWorldRot_ = matWorld_.IdentityMatrix(); // ワールド回転行列の変形もリセット
	matWorldRot_ *= matRot; // ワールド回転行列に回転を反映

	if (parent_ != nullptr)
	{
		matWorld_ *= parent_->matWorld_;//親の行列を掛け算する
		matWorldRot_ *= parent_->matWorldRot_;//親の回転行列も掛け算する
	}

	//定数バッファに転送
	constMapTransform->matWorld = matWorld_.ConvertToXMMatrix() * matView * matProjection;
}