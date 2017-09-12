#include "Window.h"

#include "resource.h"
#include "Error.h"

Window::Window()
{
}

Window::~Window()
{
}

void Window::Create(HINSTANCE instance, LPCSTR class_name, WNDPROC procedure, LPCSTR caption, LPVOID param)
{
	_class.cbSize = sizeof(WNDCLASSEX);
	_class.style = CS_OWNDC;
	_class.lpfnWndProc = procedure;
	_class.cbClsExtra = 0;
	_class.cbWndExtra = 0;
	_class.hInstance = instance;
	_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	_class.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
	_class.lpszClassName = class_name;
	_class.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!::RegisterClassEx(&_class))
		ERROR_MSG("Unable to register window class");

	_window = ::CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, class_name, caption, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, instance, param);

	if (!_window)ERROR_MSG("Unable to create window");
}

/*HWND Window::_MakeToolbar(HINSTANCE instance) //Commctrl toolbar
{
	HWND toolbar = ::CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD, 0, 0, 0, 0, _window, (HMENU)IDR_MENU, instance, NULL);

	if (!toolbar) ERROR_MSG("Toolbar fail");

	::SendMessage(toolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	::SendMessage(toolbar, TB_SETBITMAPSIZE, (WPARAM)0, (LPARAM)MAKELONG(32, 32));

	TBADDBITMAP addbmp;
	TBBUTTON toolbar_buttons[3];

	::ZeroMemory(toolbar_buttons, sizeof(toolbar_buttons));

	char wew[] = "wews";

	toolbar_buttons[0].iBitmap = I_IMAGENONE;
	toolbar_buttons[0].iString = wew[0];
	toolbar_buttons[0].fsStyle = TBSTYLE_BUTTON;
	toolbar_buttons[0].fsState = TBSTATE_ENABLED;

	::SendMessage(toolbar, TB_ADDBUTTONS, 1, (LPARAM)&toolbar_buttons);
	::ShowWindow(toolbar, SW_SHOW);

	addbmp.hInst = instance;


	return toolbar;
}*/

void Window::Show(int cmd_show) {
	::ShowWindow(_window, cmd_show);
}
