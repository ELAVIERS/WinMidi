#pragma once
#include <Windows.h>

class Window
{
public:
	Window();
	~Window();

	void Initialise(LPCTSTR ClassName, HINSTANCE Instance, LPCTSTR Title, LPVOID Parameter);
	void Show(int CmdShow);

	inline HWND GetHandle() { return _hwnd; };
	inline HMENU GetMenu() { return ::GetMenu(_hwnd); };
private:
	HWND	_hwnd;
};
