#include "WinMidi.h"

#include "resource.h"
#include "AboutDialog.h"
#include "Midi.h"

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

	//Direct2D
	::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &_d2d_factory);

	RECT rect;
	::GetClientRect(_window.GetHandle(), &rect);

	_d2d_factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(_window.GetHandle(), D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top)),
		&_d2d_render_target);

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

			::TranslateMessage(&msg);
			::DispatchMessage (&msg);
		}

		_timer.Start();
		{
			char s[50];
			snprintf(s, 50, "Tick %d", _player.GetCurrentTick());
			::SetWindowText(_window.GetHandle(), s);

			_player.Update(delta_seconds);

			_Render((float)delta_seconds);
		}
		delta_seconds = _timer.Stop();
	}
}

void WinMidi::_Render(float delta_seconds)
{
	static ID2D1SolidColorBrush *brush;
	if (!brush)_d2d_render_target->CreateSolidColorBrush(D2D1::ColorF(0x884400), &brush);

	if (_d2d_render_target)
	{
		_d2d_render_target->BeginDraw();
		_d2d_render_target->Clear(D2D1::ColorF(0));
		
		_note_sheet.Render(_d2d_render_target, brush, _player.GetCurrentTick());
		
		_d2d_render_target->EndDraw();
	}
}

void WinMidi::Command(int id)
{
	switch (id)
	{
	case ID_FILE_PLAY:
		_player.Play();
		break;
	case ID_FILE_PAUSE:
		_player.Pause();
		break;
	case ID_FILE_STOP:
		_player.Stop();
		break;
	case ID_FILE_OPEN:
		_file.LoadFromDirectory(_window.GetHandle());

		_note_sheet.Load(_file.GetTracks());

		_player.SetFile(&_file);
		_player.Stop();
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
		_Render(0); //Redraw the frame
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

	WinMidi *win_midi = (WinMidi*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch (msg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	case WM_CLOSE:
		::DestroyWindow(hWnd);
		break;

	case WM_COMMAND:
		win_midi->Command(LOWORD(wParam));
		break;
	case WM_SIZE:
		win_midi->Resize(LOWORD(lParam), HIWORD(lParam));
		break;
	default:
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}
