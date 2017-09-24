#include "WinMidi.h"

#include "Error.h"
#include "AboutDialog.h"
#include "FileDialogManager.h"
#include "Midi.h"
#include "resource.h"

#include <windowsx.h>

WinMidi::WinMidi()
{
}


WinMidi::~WinMidi()
{
}

HCURSOR arrow	= ::LoadCursor(NULL, IDC_ARROW);
HCURSOR grabber = ::LoadCursor(NULL, IDC_HAND);

void WinMidi::Initialise(HINSTANCE instance)
{
	//Variables
	_tick_marker_enabled = _progress_bar_enabled = true;
	_flip_axes = false;

	//Create window
	_window_class.Initialise("WinMidi_Window", instance, MAKEINTRESOURCE(IDR_MENU), _WindowProcedure);
	_window.Initialise("WinMidi_Window", instance, "WinMidi", this);
	_accelerators = ::LoadAccelerators(instance, MAKEINTRESOURCE(IDR_ACCELERATORS));

	SetMenuItemChecked(ID_VIEW_TICKMARKER, _tick_marker_enabled);
	SetMenuItemChecked(ID_VIEW_PROGRESSBAR, _progress_bar_enabled);
	SetMenuItemChecked(ID_VIEW_VERTICAL, _flip_axes);
	//Direct2D
	::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &_d2d_factory);

	RECT rect;
	::GetClientRect(_window.GetHandle(), &rect);

	_d2d_factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(_window.GetHandle(), 
			D2D1::SizeU(),
			D2D1_PRESENT_OPTIONS_IMMEDIATELY),
		&_d2d_render_target);

	_d2d_render_target->CreateSolidColorBrush(D2D1::ColorF(0), &_brush);

	Resize(rect.right - rect.left, rect.bottom - rect.top);

	//Misc
	_tick_percentage = 0.5f;
	_CalculateTickOffset();
	_note_sheet.SetPixelsPerCrotchet(256);
}

void WinMidi::LoadMIDIFile(const char* file, bool play)
{
	_tempo_multiplier = 1.f;
	_file.LoadFromFile(file);
	_note_sheet.Load(_file.GetTracks());
	_note_sheet.SetTicksPerCrotchet(_file.GetTicksPerCrotchet());
	_player.SetFile(&_file);

	if(play)
		_player.Play();
}

HRESULT WinMidi::Run(int cmd_show)
{
	_window.Show(cmd_show);
	
	MSG		msg;
	
	while (1)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			if (!::TranslateAccelerator(_window.GetHandle(), _accelerators, &msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		else _Frame();
	}

	_brush->Release();
	_d2d_render_target->Release();
	_d2d_factory->Release();

	return (HRESULT)msg.wParam;
}

void WinMidi::_Frame()
{
	static double delta_seconds = 0.0;
	_timer.Start();
	_Update(delta_seconds);
	_Render();
	delta_seconds = _timer.Stop();
	_run_time += (float)delta_seconds;
}

void WinMidi::_Update(double delta_seconds)
{
	_player.Update(delta_seconds * _tempo_multiplier);

	static char s[64];
	snprintf(s, 64, "Tick %u, Note Length:%d, Speed Multiplier:%.2f", _player.GetCurrentTick(), _note_sheet.GetPixelsPerCrotchet(), _tempo_multiplier);
	::SetWindowText(_window.GetHandle(), s);
}

void WinMidi::_Render()
{
		_d2d_render_target->BeginDraw();
		_d2d_render_target->Clear(D2D1::ColorF(0x222222));
		
		//Draw Notes and reset transform
		_note_sheet.Render(_d2d_render_target, _brush, _player.GetCurrentTick());
		_d2d_render_target->SetTransform(D2D1::Matrix3x2F::Identity());

		//Draw UI
		if (_tick_marker_enabled)
		{
			_brush->SetColor(D2D1::ColorF(0xFFFFFF));
			if (!_flip_axes)
				_d2d_render_target->DrawLine(D2D1::Point2F((float)_tick_offset, 0),
					D2D1::Point2F((float)_tick_offset, (float)_note_sheet.GetSize().height),
					_brush, .25f);
			else
				_d2d_render_target->DrawLine(D2D1::Point2F(0, (float)(_tick_offset)),
					D2D1::Point2F((float)_note_sheet.GetSize().width, (float)(_tick_offset)),
					_brush, .25f);
		}

		if (_progress_bar_enabled)
		{
			_brush->SetColor(D2D1::ColorF(0));
			_d2d_render_target->FillRectangle(_progress_rect_bg, _brush);
			_brush->SetColor(D2D1::ColorF(0x00AA00));
			_progress_rect.right = _window_size.width * ((float)_player.GetCurrentTick() / (float)_note_sheet.GetLength());
			_d2d_render_target->FillRectangle(_progress_rect, _brush);
		}

		_d2d_render_target->EndDraw();
}
//////////////////////////////////////

void WinMidi::SetMenuItemChecked(int id, bool check)
{
	static char text[32];

	::GetMenuString(_window.GetMenu(), id, text, 32, MF_BYCOMMAND);
	::ModifyMenu(_window.GetMenu(), id, MF_BYCOMMAND | (check ? MF_CHECKED : MF_UNCHECKED), id, text);
}

void WinMidi::_CalculateTickOffset()
{
	unsigned short AreaWidth = 8;

	if (_flip_axes)
	{
		_tick_offset = (short)(_note_sheet.GetSize().height * _tick_percentage);
		_region_tick_marker.Set(0, _tick_offset > AreaWidth ? _tick_offset - AreaWidth : 0,
			_note_sheet.GetSize().width, _tick_offset + AreaWidth);
	}
	else
	{
		_tick_offset = (short)(_note_sheet.GetSize().width  * _tick_percentage);
		_region_tick_marker.Set(_tick_offset > AreaWidth ? _tick_offset - AreaWidth : 0, 0, 
			_tick_offset + AreaWidth, _note_sheet.GetSize().height);
	}

	_note_sheet.SetTickOffset(_tick_offset);
}

void WinMidi::_ToggleFullscreen()
{
	static bool		fullscreen = false;
	static bool		maximised;
	static RECT		winSize;
	static HMENU	menu;

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

		menu = _window.GetMenu();
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

void WinMidi::Resize(unsigned int width, unsigned int height)
{
	_window_size = D2D1::SizeU(width, height);

	_UpdateSizes();
}

void WinMidi::_UpdateSizes()
{
	_note_sheet.Resize(D2D1::SizeU(_window_size.width, _window_size.height - (_progress_bar_enabled ? 32 : 0)));

	_progress_rect.top = _progress_rect_bg.top = (float)_note_sheet.GetSize().height;
	_progress_rect.bottom = _progress_rect_bg.bottom = (float)_window_size.height;
	_progress_rect_bg.right = (float)_window_size.width;

	_region_progress_bar.Set(0, _note_sheet.GetSize().height, _window_size.width, _window_size.height);

	_CalculateTickOffset();

	if (_d2d_render_target) {
		_d2d_render_target->Resize(_window_size);
		_Render(); //Redraw the frame
	}
}

void WinMidi::MouseMove(short x, short y)
{
	_cursor_pos = { x, y };

	_MouseUpdate();
}

void WinMidi::MouseDown()
{
	if (_region_progress_bar_overlap)
	{
		_region_progress_bar_hold = true;
		_was_playing = _player.IsPlaying();
		_player.Pause();
	}
	else if (_region_tick_marker_overlap)
		_region_tick_marker_hold = true;

	_MouseUpdate();
}

void WinMidi::MouseUp()
{
	_region_progress_bar_hold = _region_tick_marker_hold = false;

	if (_was_playing)
		_player.Play();
}

void WinMidi::_MouseUpdate()
{
	//If Cursor is over region change it
	if (_progress_bar_enabled && _region_progress_bar.Overlaps((short)_cursor_pos.x, (short)_cursor_pos.y))
	{
		_region_progress_bar_overlap = true;
		::SetCursor(grabber);
	}
	else
	{
		_region_progress_bar_overlap = false;

		if (_tick_marker_enabled && _region_tick_marker.Overlaps((short)_cursor_pos.x, (short)_cursor_pos.y))
		{
			_region_tick_marker_overlap = true;
			::SetCursor(grabber);
		}
		else
		{
			_region_tick_marker_overlap = false;
			::SetCursor(arrow);
		}
	}

	if (_region_progress_bar_hold )
	{
		if (_player.IsPlayable())
			_player.UpdateTicks((_note_sheet.GetLength() * ((float)_cursor_pos.x / (float)_window_size.width)) - _player.GetCurrentTick());
	}
	else if (_region_tick_marker_hold)
	{
		if (_flip_axes)
			_tick_percentage = (float)_cursor_pos.y / (float)_note_sheet.GetSize().height;
		else
			_tick_percentage = (float)_cursor_pos.x / (float)_note_sheet.GetSize().width;

		if (_tick_percentage > .49f && _tick_percentage < .51f)
			_tick_percentage = 0.5f;
		else if (_tick_percentage < .01f)
			_tick_percentage = 0.f;
		else if (_tick_percentage > .99f)
			_tick_percentage = 1.f;

		_CalculateTickOffset();
	}
}

void WinMidi::Command(int id)
{
	switch (id)
	{
	case ID_FILE_OPEN:
	case IDA_OPEN:
	{
		std::string file_path;
		if (FileDialogManager::OpenFile(file_path))
			LoadMIDIFile(file_path.c_str());
		break;
	}

	case ID_FILE_PLAY:
	case IDA_PLAY:
		_player.Toggle();
		break;

	case ID_FILE_STOP:
	case IDA_STOP:
		_player.Stop();
		break;

	case ID_VIEW_TICKMARKER:
	case IDA_TICKMARKER:
		_tick_marker_enabled = !_tick_marker_enabled;

		SetMenuItemChecked(ID_VIEW_TICKMARKER, _tick_marker_enabled);
		break;

	case ID_VIEW_PROGRESSBAR:
	case IDA_PROGRESSBAR:
		_progress_bar_enabled = !_progress_bar_enabled;
		_UpdateSizes();

		SetMenuItemChecked(ID_VIEW_PROGRESSBAR, _progress_bar_enabled);
		break;

	case ID_VIEW_FULLSCREEN:
	case IDA_FULLSCREEN:
		_ToggleFullscreen();
		break;

	case ID_VIEW_VERTICAL:
	case IDA_VERTICAL:
		_flip_axes = !_flip_axes;
		_CalculateTickOffset();

		_note_sheet.SetFlipAxes(_flip_axes);

		SetMenuItemChecked(ID_VIEW_VERTICAL, _flip_axes);
		break;
	case ID_TOOLS_DUMP:
		_file.DisplayStringToFile("events.txt");
		::ShellExecute(NULL, "open", "events.txt", NULL, NULL, SW_SHOWMAXIMIZED);
		break;
	case ID_HELP_ABOUT:
		AboutDialog::Open(NULL, _window.GetHandle());
		break;

	case IDA_QUIT:
		PostQuitMessage(0);
		break;
	case IDA_NOTEPLUS:
		_note_sheet.SetPixelsPerCrotchet(_note_sheet.GetPixelsPerCrotchet() + 8);
		break;
	case IDA_NOTEMINUS:
		_note_sheet.SetPixelsPerCrotchet(_note_sheet.GetPixelsPerCrotchet() - 8);
		break;
	case IDA_TEMPOPLUS:
		_tempo_multiplier += 0.25f;
		break;
	case IDA_TEMPOMINUS:
		_tempo_multiplier -= 0.25f;
		if (_tempo_multiplier < 0.f) _player.ResetNotes();
		break;
	case IDA_SKIPFWD:
		_player.Seek(5.0);
		break;
	case IDA_SKIPBACK:
		_player.Seek(-5.0);
		break;
	}
}

//Static window procedure
#define WINMIDI ((WinMidi*)::GetWindowLongPtr(hWnd, GWLP_USERDATA))
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

	case WM_MOUSEMOVE:
		WINMIDI->MouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_LBUTTONDOWN:
		WINMIDI->MouseDown();
		break;
	case WM_LBUTTONUP:
		WINMIDI->MouseUp();
		break;

	case WM_COMMAND:
		WINMIDI->Command(LOWORD(wParam));
		break;
	case WM_SIZE:
		WINMIDI->Resize	(LOWORD(lParam), HIWORD(lParam));
		break;
	default:
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 1;
}
