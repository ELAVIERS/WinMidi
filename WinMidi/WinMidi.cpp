#include "WinMidi.h"

#include "resource.h"
#include "AboutDialog.h"

WinMidi::WinMidi()
{
}


WinMidi::~WinMidi()
{
}

void WinMidi::Initialise(HINSTANCE instance)
{
	_instance = instance;

	_window.Create(instance, "WinMidi_Window", WindowProcedure, "WinMidi", this);

	::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &_d2d_factory);

	RECT rect;
	::GetClientRect(_window.GetHandle(), &rect);

	_d2d_factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(_window.GetHandle(), D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top)),
		&_d2d_render_target);

}

HRESULT WinMidi::Run(int cmd_show)
{
	_window.Show(cmd_show);
	
	float delta_seconds = 0;

	while (1)
	{
		_timer.start();

		if (!HandleMessages())
			break;

		Render(delta_seconds);

		delta_seconds = _timer.stop();
	}

	return (HRESULT)_message.wParam;
}

bool WinMidi::HandleMessages()
{
	while (::PeekMessage(&_message, NULL, 0, 0, PM_REMOVE))
	{
		if (_message.message == WM_QUIT)
			return false;
		::TranslateMessage(&_message);
		::DispatchMessage(&_message);
	}

	return true;
}

void WinMidi::Render(float delta_seconds)
{	//Messy!
	static D2D1_RECT_F rect_base = D2D1::RectF(16, 16, 128, 128);
	static D2D1_ROUNDED_RECT rect = D2D1::RoundedRect(rect_base, 8, 8);

	if (_d2d_render_target)
	{
		static ID2D1SolidColorBrush *brush;
		if (!brush)_d2d_render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &brush);

		_d2d_render_target->BeginDraw();

		_d2d_render_target->Clear(D2D1::ColorF(D2D1::ColorF::Black));

		_d2d_render_target->FillRoundedRectangle(rect, brush);

		_d2d_render_target->EndDraw();
	}
}

void WinMidi::Command(int id)
{
	switch (id)
	{
	case ID_FILE_OPEN:
		_file.LoadFromDirectory(_window.GetHandle());
		break;
	case ID_TOOLS_DUMP:
		_file.DisplayStringToFile("out.txt");
		::ShellExecute(NULL, "open", "out.txt", NULL, NULL, SW_SHOWMAXIMIZED);
		break;
	case ID_HELP_ABOUT:
		AboutDialog::Open(_instance, _window.GetHandle());
		break;
	}
}

void WinMidi::Resize(unsigned int width, unsigned int height)
{
	if (_d2d_render_target) {
		_d2d_render_target->Resize(D2D1::SizeU(width, height));
		Render(0); //Redraw the frame
	}
}

//Static window procedure
LRESULT CALLBACK WinMidi::WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
