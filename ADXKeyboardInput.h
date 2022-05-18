#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <cassert>


class ADXKeyBoardInput
{
public:
	//キーボードデバイスの生成
	IDirectInputDevice8* keyboard;
	//全キーの入力状態を入れる変数
	BYTE key[256] = {};
	//前のフレームの入力状態を入れる変数
	BYTE prevKey[256] = {};

	ADXKeyBoardInput(HRESULT& result, const GUID& GUID_SysKeyboard,
		const DIDATAFORMAT* c_dfDIKeyboard, IDirectInput8W* directInput,
		HWND hwnd, DWORD dword);
	void Update();
	bool KeyTrigger(int keyNum);
	bool KeyRelease(int keyNum);
};