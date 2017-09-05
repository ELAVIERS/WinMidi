#include <Windows.h>
#include <gl\GL.h>
#include <fstream> //For event dump

#include "Timer.h"
#include "Window.h"
#include "resource.h"
#include "MidiFile.h"

float run_time = 0.f;

HINSTANCE instance;
HDC hDvc;
Timer timer;
MidiFile file;

//Forward declarations
void render();
LRESULT CALLBACK AboutProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prev, LPSTR cmd_str, int cmd_show)
{
	::instance = instance;

	Window window(instance, "MainWindow", WinProc, "Hello");
	HWND	hWnd = window.GetHandle();
		  ::hDvc = ::GetDC(hWnd);
	HGLRC	hGLContext = wglCreateContext(hDvc);

	wglMakeCurrent(hDvc, hGLContext);
	window.Show(cmd_show);

	bool running;
	float delta_time = 0.f;
	MSG msg;

	do {
		timer.start();
		static char str[64];
		snprintf(str, sizeof(str), "%f, %f FPS, %f delta", run_time, 1.f / delta_time, delta_time);
		::SetWindowText(hWnd, str);

		while (::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT) break;

		render();

		delta_time = timer.stop();
		run_time += delta_time;
	} while (1);

	wglMakeCurrent(NULL, NULL);
	::ReleaseDC(hWnd, hDvc);
	wglDeleteContext(hGLContext);
	
	return (int)msg.wParam;
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(std::sin(run_time), std::sin(run_time + (3.1415 / 2)), std::sin(run_time + 3.1415));
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
	case WM_SIZE:
		glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
		render(); //Redraw frame (might cause problems later)
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

			ofile << file.GetDisplayString();

			ShellExecute(NULL, "open", "out.txt", NULL, NULL, SW_SHOWMAXIMIZED);
			break;
		}

		case ID_FILE_OPEN:
			file.LoadFromDirectory(hWnd);
			break;
		}
		break;

	case WM_CLOSE:
		::DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	default:
		return ::DefWindowProc(hWnd, Msg, wParam, lParam);
	}

	return 0;
}
