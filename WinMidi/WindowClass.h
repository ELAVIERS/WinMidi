#pragma once
#include <windows.h>

class WindowClass
{
public:
	WindowClass();
	~WindowClass();

	void Initialise(LPCTSTR ClassName, HINSTANCE Instance, LPCTSTR Menu, WNDPROC WindowProcedure);
private:
	LPCTSTR _name;
	HINSTANCE _instance;
};
