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

	inline void SetTicksPerCrotchet(unsigned short TicksPerCrotchet) { _ticks_per_crotchet = TicksPerCrotchet; _UpdateScaleMatrix(); };
	inline void SetPixelsPerCrotchet(signed short PixelsPerCrotchet) { _pixels_per_crotchet = PixelsPerCrotchet; _UpdateScaleMatrix(); };

	inline unsigned char getMaximumPitch() { return _max_pitch; };
	inline unsigned char getMinimumPitch() { return _min_pitch; };
protected:
	std::vector<Note>*			_notes;
	std::vector<D2D1_RECT_F>*	_rects;
	unsigned short				_track_count;

	unsigned char				_max_pitch;
	unsigned char				_min_pitch;
	int							_x_offset;


	void _UpdateScaleMatrix();
	D2D1_MATRIX_3X2_F			_scale_mat;

	D2D1_SIZE_U					_size;

	unsigned short				_ticks_per_crotchet;
	signed short				_pixels_per_crotchet;

	void _BakeRects();
};

