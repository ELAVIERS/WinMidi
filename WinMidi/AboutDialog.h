#pragma once
#include <Windows.h>

namespace AboutDialog //Functions that handle the about dialog
{
	void Open(HINSTANCE instance, HWND owner);

	INT_PTR CALLBACK Procedure(HWND, UINT, WPARAM, LPARAM);
}
