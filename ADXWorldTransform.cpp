#include "ADXWorldTransform.h"
#include <math.h>

void ADXWorldTransform::Initialize()
{
	matWorld_ = matWorld_.IdentityMatrix();

}

void ADXWorldTransform::UpdateMatrix(XMMATRIX matView, XMMATRIX matProjection)
{
	//////�g�k//////
	//�X�P�[�����O�{�����s��ɐݒ肷��
	ADXMatrix4 matScale =
	{ scale_.x,0,0,0,
	0,scale_.y,0,0,
	0,0,scale_.z,0,
	0,0,0,1 };

	//////��]//////
	//Z����]�s���錾
	ADXMatrix4 matRotZ =
	{ (float)cos(rotation_.z),(float)sin(rotation_.z),0,0,
	(float)-sin(rotation_.z),(float)cos(rotation_.z),0,0,
	0,0,1,0,
	0,0,0,1 };
	//X����]�s���錾
	ADXMatrix4 matRotX =
	{ 1,0,0,0,
	0,(float)cos(rotation_.x),(float)sin(rotation_.x),0,
	0,(float)-sin(rotation_.x),(float)cos(rotation_.x),0,
	0,0,0,1 };
	//Y����]�s���錾
	ADXMatrix4 matRotY =
	{ (float)cos(rotation_.y),0,(float)-sin(rotation_.y),0,
	0,1,0,0,
	(float)sin(rotation_.y),0,(float)cos(rotation_.y),0,
	0,0,0,1 };

	//�����p��]�s���錾���AZXY�̏��ɍ���
	ADXMatrix4 matRot = matRotZ;
	matRot *= matRotX;
	matRot *= matRotY;

	//////���s�ړ�//////
	//�ړ��ʂ��s��ɐݒ肷��
	ADXMatrix4 matTrans =
	{ 1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	translation_.x,translation_.y,translation_.z,1 };


	//�P�ʍs�����
	matWorld_ = matWorld_.IdentityMatrix();
	//�X�P�[�����O�s����|����
	matWorld_ *= matScale;
	//�����p��]�s����|����
	matWorld_ *= matRot;
	//���s�ړ��s����|����
	matWorld_ *= matTrans;


	//�萔�o�b�t�@�ɓ]��
	constMapTransform->matWorld = matWorld_.ConvertToXMMatrix() * matView * matProjection;
}