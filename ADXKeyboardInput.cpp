#include "ADXKeyboardInput.h"

ADXKeyBoardInput::ADXKeyBoardInput(HRESULT& result,const GUID& GUID_SysKeyboard,
	const DIDATAFORMAT* c_dfDIKeyboard, IDirectInput8W* directInput,
	HWND hwnd,DWORD dword)
{
	keyboard = nullptr;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard->SetDataFormat(c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		hwnd, dword);
	assert(SUCCEEDED(result));
}

void ADXKeyBoardInput::Update()
{
	//前のフレームの入力状態を取得する
	for (int i = 0; i < sizeof(key) / sizeof(*key); i++)
	{
		prevKey[i] = key[i];
	}

	//全キーの入力状態を取得する
	keyboard->GetDeviceState(sizeof(key), key);
}

bool ADXKeyBoardInput::KeyTrigger(int keyNum)
{
	return key[keyNum] && !prevKey[keyNum];
}

bool ADXKeyBoardInput::KeyRelease(int keyNum)
{
	return !key[keyNum] && prevKey[keyNum];
}