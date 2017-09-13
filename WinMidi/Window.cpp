#include "Window.h"

#include "Error.h"

Window::Window()
{
}


Window::~Window()
{
}

void Window::Initialise(LPCTSTR class_name, HINSTANCE instance, LPCTSTR caption, LPVOID param)
{
	_hwnd = ::CreateWindowEx(0, class_name, caption, WS_OVERLAPPEDWINDOW,	//Exstyle, class name, window name, style
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,									//x,y,width,height 
		NULL, NULL, instance,												//Parent, Menu, Instance
		param);																//LParam

	if (!_hwnd)
		Error::ErrorMessageFatal("Could not create window");
}

void Window::Show(int cmd_show)
{
	::ShowWindow(_hwnd, cmd_show);
}
