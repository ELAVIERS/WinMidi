#pragma once
#include <Windows.h>

class Window
{
public:
	Window(HINSTANCE Instance, LPCSTR ClassName, WNDPROC Procedure, LPCSTR Caption);
	~Window();

	void show(int CmdShow);

	HWND get_handle() { return _window; };
protected:
	WNDCLASSEX _class;
	HWND _window;

	HWND make_toolbar(HINSTANCE);
};

