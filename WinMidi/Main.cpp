#include <Windows.h>
#include <gl\GL.h>
#include <fstream> //For event dump

#include "Window.h"
#include "resource.h"
#include "MidiFile.h"

HINSTANCE instance;
HDC hDvc;
MidiFile file;

//Forward declarations
LRESULT CALLBACK AboutProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prev, LPSTR cmd_str, int cmd_show)
{
	::instance = instance;

	Window window(instance, "MainWindow", WinProc, "Hello");
	HWND	hWnd = window.get_handle();
		  ::hDvc = GetDC(hWnd);
	HGLRC	hGLContext = wglCreateContext(hDvc);

	wglMakeCurrent(hDvc, hGLContext);
	window.show(cmd_show);

	MSG msg;
	while (::GetMessage(&msg, NULL, 0, 0) > 0)
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	wglMakeCurrent(NULL, NULL);
	ReleaseDC(hWnd, hDvc);
	wglDeleteContext(hGLContext);

	return (int)msg.wParam;
}

void redraw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3ub(255, 127, 0);
	glRectf(-.9f, -.9f, .9f, .9f);
	SwapBuffers(hDvc);

}

LRESULT CALLBACK AboutProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			::EndDialog(hWnd, LOWORD(wParam));
			return TRUE;
		}
		break;
	}

	return FALSE;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	static PAINTSTRUCT paint;

	switch (Msg)
	{
	case WM_PAINT:
		redraw();

		BeginPaint(hWnd, &paint);
		EndPaint(hWnd, &paint);
		break;
	case WM_SIZE:
		glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
		redraw();
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_HELP_ABOUT:
			::DialogBox(instance, MAKEINTRESOURCE(IDD_ABOUT), hWnd, AboutProc);
			break;

		case ID_TOOLS_DUMP:
		{
			std::ofstream ofile;
			ofile.open("out.txt", std::ios::out);

			ofile << file.getDisplayString();

			ShellExecute(NULL, "open", "out.txt", NULL, NULL, SW_SHOWMAXIMIZED);
			break;
		}

		case ID_FILE_OPEN:
			file.loadFromDirectory(hWnd);
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return ::DefWindowProc(hWnd, Msg, wParam, lParam);
	}

	return 0;
}
