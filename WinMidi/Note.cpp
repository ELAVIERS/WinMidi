#include "Note.h"
#include "Midi.h"
#include "MidiFile.h"


Note::Note()
{
}


Note::~Note()
{
}

using namespace std;

const vector<Note> Note::GetNotes(const MidiFile& file)
{
	vector<Note> notes;
	const vector<MidiTrack*>& tracks = file.GetTracks();
	unsigned int tick = 0;

	for (MidiTrack* track : tracks)
	{
		tick = 0;
		const vector<Event*>& events = track->getEvents();
		for (Event* event : events)
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
					notes.push_back(new_note);
				}
				else if (op == Events::Midi::NoteOff || (op == Events::Midi::NoteOn && !vel))
				{
					for (Note& note : notes)
					{
						if (note.pitch == event->GetData()[0] && !note.end)
							note.end = tick;
					}
				}
			}
		}


	}

	return notes;
}

