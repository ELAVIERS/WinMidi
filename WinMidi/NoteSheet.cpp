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
}

using namespace std;

void NoteSheet::Load(const vector<MidiTrack*>& tracks)
{
	delete[] _notes;
	_track_count = (unsigned short)tracks.size();
	_notes = new vector<Note>[_track_count];

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
					new_note.Start = tick;
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
					for (Note& note : _notes[i])
					{
						if (note.pitch == event->GetData()[0] && !note.end)
							note.end = tick;
					}
				}
			}
		}
	}
}

void NoteSheet::Render(ID2D1HwndRenderTarget* render_target, ID2D1SolidColorBrush* brush, float tick)
{
	static D2D1_COLOR_F color;

	float row_width = (float)_size.height / (float)(_max_pitch - _min_pitch);

	for (int i = 0; i < _track_count; i++)
	{
		for (const Note& note : _notes[i])
		{
			if (note.end + _x_offset < tick || note.Start + _x_offset >(tick + _size.width))
				continue;

			if (tick >= note.Start && tick <= note.end)
				brush->SetColor(D2D1::ColorF(0xFFFFFF));
			else
				brush->SetColor(TRACK_COLOURS[i % TRACK_COLOUR_COUNT]);

			render_target->FillRectangle(D2D1::RectF(
				(float)(signed int)(note.Start - tick + _x_offset), _size.height - (row_width * (note.pitch - _min_pitch + 1)),
				(float)(signed int)(note.end - tick + _x_offset),	_size.height - (row_width * (note.pitch - _min_pitch))
			), brush);
		}
	}
}

void NoteSheet::Resize(const D2D1_SIZE_U& size)
{
	_size = size;

	_x_offset = _size.width / 2;
}
