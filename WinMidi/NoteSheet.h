#pragma once
#include <d2d1.h>
#include <vector>

#include "Note.h"

class MidiTrack;

class NoteSheet
{
public:
	NoteSheet();
	~NoteSheet();

	void Load(const std::vector<MidiTrack*>& Tracks);
	void Render(ID2D1HwndRenderTarget* RenderTarget, ID2D1SolidColorBrush* Brush, unsigned int Tick);
	void Resize(const D2D1_SIZE_U& Size);

	inline unsigned char getMaximumPitch() { return _max_pitch; };
	inline unsigned char getMinimumPitch() { return _min_pitch; };
protected:
	std::vector<Note>*	_notes;
	unsigned short		_track_count;

	unsigned char		_max_pitch;
	unsigned char		_min_pitch;
	int					_x_offset;

	D2D1_SIZE_U			_size;
};

