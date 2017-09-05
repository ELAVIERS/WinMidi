#pragma once
#include <Windows.h>

class Window
{
public:
	Window(HINSTANCE Instance, LPCSTR ClassName, WNDPROC Procedure, LPCSTR Caption);
	~Window();

	void Show(int CmdShow);

	HWND GetHandle() { return _window; };
protected:
	WNDCLASSEX _class;
	HWND _window;

	HWND _MakeToolbar(HINSTANCE);
};

