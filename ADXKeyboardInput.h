#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <cassert>


class ADXKeyBoardInput
{
public:
	//�L�[�{�[�h�f�o�C�X�̐���
	IDirectInputDevice8* keyboard;
	//�S�L�[�̓��͏�Ԃ�����ϐ�
	BYTE key[256] = {};
	//�O�̃t���[���̓��͏�Ԃ�����ϐ�
	BYTE prevKey[256] = {};

	ADXKeyBoardInput(HRESULT& result, const GUID& GUID_SysKeyboard,
		const DIDATAFORMAT* c_dfDIKeyboard, IDirectInput8W* directInput,
		HWND hwnd, DWORD dword);
	void Update();
	bool KeyTrigger(int keyNum);
	bool KeyRelease(int keyNum);
};