#include "NoteSheet.h"

#include "Midi.h"
#include "MidiTrack.h"
#include "Note.h"

#define COLOUR(value) D2D1::ColorF(D2D1::ColorF::value)
const D2D1::ColorF TRACK_COLOURS[] = { COLOUR(Red), COLOUR(Blue), COLOUR(Green), COLOUR(Yellow), COLOUR(Orange), COLOUR(Indigo), COLOUR(Violet), COLOUR(Pink) };
const int TRACK_COLOUR_COUNT = sizeof(TRACK_COLOURS) / sizeof(TRACK_COLOURS[0]);

NoteSheet::NoteSheet() : _track_count(0)
{
}


NoteSheet::~NoteSheet()
{
	delete[] _notes;
	delete[] _rects;
}

inline D2D1_COLOR_F ScaleColour(D2D1_COLOR_F color, float amount)
{
	return D2D1::ColorF(color.r + amount, color.g + amount, color.b + amount);
}

using namespace std;

void NoteSheet::Load(const vector<MidiTrack*>& tracks)
{
	delete[] _notes;
	_track_count = (unsigned short)tracks.size();
	_notes = new vector<Note>[_track_count];
	_length = 0;

	bool first = true;

	for (int i = 0; i < _track_count; i++)
	{
		unsigned int tick = 0;

		auto events = tracks[i]->getEvents();
		for (MidiEvent* event : events)
		{
			tick += event->delta_ticks;

			if (event->type == MIDI)
			{
				unsigned char op = event->message & 0xF0;
				unsigned char vel = event->GetData()[1];

				if (op == Events::Midi::NoteOn && vel)
				{
					Note new_note;
					new_note.start = tick;
					new_note.pitch = event->GetData()[0];
					new_note.end = 0;
					_notes[i].push_back(new_note);

					if (new_note.pitch > _max_pitch || first)
						_max_pitch = new_note.pitch;
					if (new_note.pitch < _min_pitch || first)
						_min_pitch = new_note.pitch;

					first = false;
				}
				else if (op == Events::Midi::NoteOff || (op == Events::Midi::NoteOn && !vel))
				{
					if (tick > _length)
						_length = tick;

					for (Note& note : _notes[i])
					{
						if (note.pitch == event->GetData()[0] && note.end == 0)
						{
							note.end = tick;
							break;
						}
					}
				}
			}
		}
	}

	_BakeRects();
}

void NoteSheet::_BakeRects()
{
	delete[] _rects;
	_rects = new vector<D2D1_RECT_F>[_track_count];

	for (int i = 0; i < _track_count; i++)
	{
		for (const Note& note : _notes[i])
		{ 
			_rects[i].push_back(D2D1::RectF(
				(float)(signed int)(note.start),	(float)(255 - note.pitch),
				(float)(signed int)(note.end),		(float)(254 - note.pitch)));
		}
	}
}

void NoteSheet::Render(ID2D1HwndRenderTarget* render_target, ID2D1SolidColorBrush* brush, unsigned int tick)
{
	static D2D1_COLOR_F color;

	render_target->SetTransform(
		D2D1::Matrix3x2F::Translation(-1.f * (float)tick, 0)
		* _view_matrix
		);

	unsigned int screen_ticks = ((_flip_axes ? (float)_size.height : (float)_size.width) / (float)_pixels_per_crotchet) * _ticks_per_crotchet;
	float tick_percent = (float)_tick_offset / (_flip_axes ? (float)_size.height : (float)_size.width);

	unsigned int upper_bound = tick + screen_ticks * (1.f - tick_percent);
	unsigned int lower_bound = (tick > screen_ticks * tick_percent ? tick - screen_ticks * tick_percent : 0);

	for (int trackid = 0; trackid < _track_count; trackid++)
	{
		static size_t size;
		size = _notes[trackid].size();

		brush->SetColor(TRACK_COLOURS[trackid % TRACK_COLOUR_COUNT]);

		for (unsigned int i = 0; i < size; ++i)
		{
			if (_notes[trackid][i].start >= upper_bound) break;

			if (_notes[trackid][i].end > lower_bound)
			{
				if (tick >= _notes[trackid][i].start && tick <= _notes[trackid][i].end)
				{
					brush->SetColor(ScaleColour(TRACK_COLOURS[trackid % TRACK_COLOUR_COUNT], .75f));
					render_target->FillRectangle(_rects[trackid][i], brush);
					brush->SetColor(TRACK_COLOURS[trackid % TRACK_COLOUR_COUNT]);
				}
				else
					render_target->FillRectangle(_rects[trackid][i], brush);
			}
		}
	}
}

void NoteSheet::Resize(const D2D1_SIZE_U& size)
{
	_size = size;

	_UpdateViewMatrix();
}

void NoteSheet::_UpdateViewMatrix()
{
	if (_flip_axes)
	{
		_view_matrix =
			D2D1::Matrix3x2F::Translation(-1.f * _size.height, (float)(_max_pitch - 254))
			* D2D1::Matrix3x2F::Rotation(-90.f)
			* D2D1::Matrix3x2F::Translation(0.f, -1.f * (float)(_size.height - _tick_offset))
			* D2D1::Matrix3x2F::Scale((float)_size.width / (float)(_max_pitch - _min_pitch + 1), 1.f / ((float)_ticks_per_crotchet / (float)_pixels_per_crotchet),
				D2D1::Point2F(0.f, (float)(_tick_offset)))
			* D2D1::Matrix3x2F::Scale(-1.f,1.f, D2D1::Point2F(_size.width / 2.f, 0.f));
	}
	else
	{
		_view_matrix = D2D1::Matrix3x2F::Translation((float)_tick_offset, (float)(_max_pitch - 254))
			 * D2D1::Matrix3x2F::Scale(
				1.f / ((float)_ticks_per_crotchet / (float)_pixels_per_crotchet), (float)_size.height / (float)(_max_pitch - _min_pitch + 1), 
				D2D1::Point2F((float)_tick_offset, 0.f)
		);
	}
}
