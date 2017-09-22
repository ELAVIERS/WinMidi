#pragma once
#include <Windows.h>
#include <d2d1.h>

#include "NoteSheet.h"
#include "MidiFile.h"
#include "MidiPlayer.h"
#include "RectU.h"
#include "Timer.h"
#include "Window.h"
#include "WindowClass.h"

class WinMidi
{
public:
	WinMidi();
	~WinMidi();

	void Initialise(HINSTANCE Instance);

	HRESULT Run(int CmdShow); //returns when the window receives WM_QUIT

	void Command(int id);
	void Resize(unsigned int width, unsigned int height);

	void LoadMIDIFile(const char* File, bool Play = false);

	void MouseMove(short x, short y);
	void MouseDown();
	void MouseUp();
protected:
	//D2d
	ID2D1Factory*			_d2d_factory;
	ID2D1HwndRenderTarget*	_d2d_render_target;
	ID2D1SolidColorBrush*	_brush;

	//Window
	WindowClass				_window_class;
	Window					_window;
	D2D1_SIZE_U				_window_size;
	HACCEL					_accelerators;

	//Runtime
	Timer					_timer;
	MidiFile				_file;
	MidiPlayer				_player;
	NoteSheet				_note_sheet;

	float					_run_time;
	POINT					_cursor_pos;

	float					_tick_percentage;
	unsigned short			_tick_offset;

	float					_tempo_multiplier;
	bool					_flip_axes;

	//Click Regions
	RectU					_region_tick_offset;
	bool					_region_tick_offset_overlap;
	bool					_region_tick_offset_hold;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	void _CalculateTickOffset();
	void _Frame();
	void _Update(double DeltaSeconds);
	void _Render();
	void _ToggleFullscreen();

	static LRESULT CALLBACK _WindowProcedure(HWND WindowHandle, UINT Message, WPARAM WideParam, LPARAM LongParam);
};

