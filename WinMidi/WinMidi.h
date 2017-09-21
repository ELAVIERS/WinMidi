#pragma once
#include <Windows.h>
#include <d2d1.h>

#include "NoteSheet.h"
#include "MidiFile.h"
#include "MidiPlayer.h"
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
	unsigned short			_track_count;
	unsigned int			_line_x;

	signed short			_note_length;
	float					_tempo_multiplier;
	bool					_flip_axes;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	void _Frame();
	void _Update(double DeltaSeconds);
	void _Render();
	void _ToggleFullscreen();

	static LRESULT CALLBACK _WindowProcedure(HWND WindowHandle, UINT Message, WPARAM WideParam, LPARAM LongParam);
};

