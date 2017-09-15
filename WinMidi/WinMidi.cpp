#include "WinMidi.h"

#include "Error.h"
#include "AboutDialog.h"
#include "Midi.h"
#include "resource.h"

WinMidi::WinMidi()
{
}


WinMidi::~WinMidi()
{
}

void WinMidi::Initialise(HINSTANCE instance)
{
	//Create window
	_window_class.Initialise("WinMidi_Window", instance, MAKEINTRESOURCE(IDR_MENU), _WindowProcedure);
	_window.Initialise("WinMidi_Window", instance, "WinMidi", this);
	_accelerators = ::LoadAccelerators(instance, MAKEINTRESOURCE(IDR_ACCELERATORS));

	//Direct2D
	::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &_d2d_factory);

	RECT rect;
	::GetClientRect(_window.GetHandle(), &rect);

	_d2d_factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(_window.GetHandle(), 
			D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top), 
			D2D1_PRESENT_OPTIONS_IMMEDIATELY),
		&_d2d_render_target);

	_d2d_render_target->CreateSolidColorBrush(D2D1::ColorF(0), &_brush);

	//Other
	_note_sheet.Resize(_window_size);
}

HRESULT WinMidi::Run(int cmd_show)
{
	_window.Show(cmd_show);
	
	double	delta_seconds = 0;
	MSG		msg;
	
	while (1)
	{
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				return (HRESULT)msg.wParam;

			if (!::TranslateAccelerator(_window.GetHandle(), _accelerators, &msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}

		_timer.Start();
		_Update(delta_seconds);
		_Render();
		delta_seconds = _timer.Stop();
	}

	_brush->Release();
	_d2d_render_target->Release();
	_d2d_factory->Release();
}

void WinMidi::_Update(double delta_seconds)
{
	_player.Update(delta_seconds);

	static char s[32];
	snprintf(s, 32, "Tick %u", _player.GetCurrentTick());
	::SetWindowText(_window.GetHandle(), s);
}

void WinMidi::_Render()
{
		_d2d_render_target->BeginDraw();
		_d2d_render_target->Clear(D2D1::ColorF(0x222222));
		
		_note_sheet.Render(_d2d_render_target, _brush, _player.GetCurrentTick());

		_brush->SetColor(D2D1::ColorF(0x888888));
		_d2d_render_target->DrawLine(D2D1::Point2F((float)_player.GetCurrentTick(), 0), D2D1::Point2F((float)_player.GetCurrentTick(), 255),
			_brush, 2);
		
		_d2d_render_target->EndDraw();
}

void WinMidi::_ToggleFullscreen()
{
	static bool fullscreen = false;
	static RECT winSize;
	static bool maximised;
	static HMENU menu;

	HWND hwnd = _window.GetHandle();

	fullscreen = !fullscreen;

	if (fullscreen)
	{
		maximised = ::IsZoomed(hwnd);

		HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO info = { sizeof(info) };
		if (!GetMonitorInfo(monitor, &info))
			Error::ErrorMessage("Could not obtain monitor info");

		::GetWindowRect(hwnd, &winSize);

		menu = ::GetMenu(hwnd);
		::SetMenu(hwnd, NULL);

		::SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
		::SetWindowPos(hwnd, NULL, 0, 0, info.rcMonitor.right, info.rcMonitor.bottom, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}
	else
	{
		::SetWindowLong(_window.GetHandle(), GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
	
		if (maximised)
			::SendMessage(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		else
			::SetWindowPos(_window.GetHandle(), NULL, winSize.left, winSize.top, winSize.right - winSize.left, winSize.bottom - winSize.top, 
				SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

		::SetMenu(_window.GetHandle(), menu);
	}
}

void WinMidi::Command(int id)
{
	switch (id)
	{
	case ID_FILE_OPEN:
	case IDA_OPEN:
		_file.LoadWithDialog(_window.GetHandle());

		_note_sheet.Load(_file.GetTracks());

		_player.SetFile(&_file);
		_player.Stop();
		break;

	case ID_FILE_PLAY:
	case IDA_PLAY:
		_player.Toggle();
		break;

	case ID_FILE_STOP:
	case IDA_STOP:
		_player.Stop();
		break;

	case ID_VIEW_FULLSCREEN:
	case IDA_FULLSCREEN:
		_ToggleFullscreen();
		break;

	case ID_TOOLS_DUMP:
		_file.DisplayStringToFile("out.txt");
		::ShellExecute(NULL, "open", "out.txt", NULL, NULL, SW_SHOWMAXIMIZED);
		break;
	case ID_HELP_ABOUT:
		AboutDialog::Open(NULL, _window.GetHandle());
		break;
	}
}

void WinMidi::Resize(unsigned int width, unsigned int height)
{
	_window_size = D2D1::SizeU(width, height);
	_note_sheet.Resize(_window_size);

	if (_d2d_render_target) {
		_d2d_render_target->Resize(_window_size);
		_Render(); //Redraw the frame
	}
}

//Static window procedure
LRESULT CALLBACK WinMidi::_WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_CREATE)
	{
		LPCREATESTRUCT create = (LPCREATESTRUCT)lParam;
		WinMidi *app = (WinMidi*)create->lpCreateParams;

		::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)app);

		return 1;
	}

	switch (msg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	case WM_CLOSE:
		::DestroyWindow(hWnd);
		break;

	case WM_COMMAND:
		((WinMidi*)::GetWindowLongPtr(hWnd, GWLP_USERDATA))->Command(LOWORD(wParam));
		break;
	case WM_SIZE:
		((WinMidi*)::GetWindowLongPtr(hWnd, GWLP_USERDATA))->Resize(LOWORD(lParam), HIWORD(lParam));
		break;
	default:
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}
