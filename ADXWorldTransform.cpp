#include "ADXWorldTransform.h"
#include <math.h>

void ADXWorldTransform::Initialize()
{
	matWorld_ = matWorld_.IdentityMatrix();

}

void ADXWorldTransform::UpdateMatrix(XMMATRIX matView, XMMATRIX matProjection)
{
	//////Šgk//////
	//ƒXƒP[ƒŠƒ“ƒO”{—¦‚ðs—ñ‚ÉÝ’è‚·‚é
	ADXMatrix4 matScale =
	{ scale_.x,0,0,0,
	0,scale_.y,0,0,
	0,0,scale_.z,0,
	0,0,0,1 };

	//////‰ñ“]//////
	//ZŽ²‰ñ“]s—ñ‚ðéŒ¾
	ADXMatrix4 matRotZ =
	{ (float)cos(rotation_.z),(float)sin(rotation_.z),0,0,
	(float)-sin(rotation_.z),(float)cos(rotation_.z),0,0,
	0,0,1,0,
	0,0,0,1 };
	//XŽ²‰ñ“]s—ñ‚ðéŒ¾
	ADXMatrix4 matRotX =
	{ 1,0,0,0,
	0,(float)cos(rotation_.x),(float)sin(rotation_.x),0,
	0,(float)-sin(rotation_.x),(float)cos(rotation_.x),0,
	0,0,0,1 };
	//YŽ²‰ñ“]s—ñ‚ðéŒ¾
	ADXMatrix4 matRotY =
	{ (float)cos(rotation_.y),0,(float)-sin(rotation_.y),0,
	0,1,0,0,
	(float)sin(rotation_.y),0,(float)cos(rotation_.y),0,
	0,0,0,1 };

	//‡¬—p‰ñ“]s—ñ‚ðéŒ¾‚µAZXY‚Ì‡‚É‡¬
	ADXMatrix4 matRot = matRotZ;
	matRot *= matRotX;
	matRot *= matRotY;

	//////•½sˆÚ“®//////
	//ˆÚ“®—Ê‚ðs—ñ‚ÉÝ’è‚·‚é
	ADXMatrix4 matTrans =
	{ 1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	translation_.x,translation_.y,translation_.z,1 };


	//’PˆÊs—ñ‚ð‘ã“ü
	matWorld_ = matWorld_.IdentityMatrix();
	//ƒXƒP[ƒŠƒ“ƒOs—ñ‚ðŠ|‚¯‚é
	matWorld_ *= matScale;
	//‡¬—p‰ñ“]s—ñ‚ðŠ|‚¯‚é
	matWorld_ *= matRot;
	//•½sˆÚ“®s—ñ‚ðŠ|‚¯‚é
	matWorld_ *= matTrans;


	//’è”ƒoƒbƒtƒ@‚É“]‘—
	constMapTransform->matWorld = matWorld_.ConvertToXMMatrix() * matView * matProjection;
}