#pragma once
#include <Windows.h>

class Window
{
public:
	Window();
	~Window();

	void Create(HINSTANCE Instance, LPCSTR ClassName, WNDPROC Procedure, LPCSTR Caption, LPVOID param);
	void Show(int CmdShow);

	HWND GetHandle() { return _window; };
protected:
	WNDCLASSEX _class;
	HWND _window;

	HWND _MakeToolbar(HINSTANCE);
};

