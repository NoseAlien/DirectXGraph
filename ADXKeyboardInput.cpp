#include "ADXKeyboardInput.h"

ADXKeyBoardInput::ADXKeyBoardInput(HRESULT& result,const GUID& GUID_SysKeyboard,
	const DIDATAFORMAT* c_dfDIKeyboard, IDirectInput8W* directInput,
	HWND hwnd,DWORD dword)
{
	keyboard = nullptr;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//���̓f�[�^�`���̃Z�b�g
	result = keyboard->SetDataFormat(c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	//�r�����䃌�x���̃Z�b�g
	result = keyboard->SetCooperativeLevel(
		hwnd, dword);
	assert(SUCCEEDED(result));
}

void ADXKeyBoardInput::Update()
{
	//�O�̃t���[���̓��͏�Ԃ��擾����
	for (int i = 0; i < sizeof(key) / sizeof(*key); i++)
	{
		prevKey[i] = key[i];
	}

	//�S�L�[�̓��͏�Ԃ��擾����
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