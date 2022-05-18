#pragma once

#include <windows.h>

class ADXWindow
{
public:
	WNDCLASSEX w{};
	RECT wrc{};
	HWND hwnd;

	ADXWindow(const int window_width, const int window_height,
		LPCTSTR window_title, WNDPROC lpfnWndProc_);
};