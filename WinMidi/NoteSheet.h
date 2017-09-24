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

	inline void SetTicksPerCrotchet(unsigned short TicksPerCrotchet)	{ _ticks_per_crotchet = TicksPerCrotchet;	_UpdateViewMatrix(); };
	inline void SetPixelsPerCrotchet(signed short PixelsPerCrotchet)	{ _pixels_per_crotchet = PixelsPerCrotchet; _UpdateViewMatrix(); };
	inline void					SetTickOffset(unsigned int TickOffset)	{ _tick_offset = TickOffset;				_UpdateViewMatrix(); };
	inline void					SetFlipAxes(bool FlipAxes)				{ _flip_axes = FlipAxes;					_UpdateViewMatrix(); };

	inline signed short			GetPixelsPerCrotchet()					{ return _pixels_per_crotchet; };
	inline const D2D1_SIZE_U&	GetSize()								{ return _size; };
	inline unsigned long int	GetLength()								{ return _length; };
protected:
	unsigned long int			_length;
	//Tick of the last note off

	std::vector<Note>*			_notes;
	std::vector<D2D1_RECT_F>*	_rects;
	unsigned short				_track_count;

	unsigned char				_max_pitch;
	unsigned char				_min_pitch;

	D2D1_MATRIX_3X2_F			_view_matrix;

	D2D1_SIZE_U					_size;

	unsigned short				_ticks_per_crotchet;
	signed short				_pixels_per_crotchet;
	unsigned int				_tick_offset;

	bool						_flip_axes;

	void _BakeRects();
	void _UpdateViewMatrix();
};
