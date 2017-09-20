#include "MidiTrack.h"

#include "Midi.h"
#include "MidiEvent.h"
#include "MidiFileUtils.h"

MidiTrack::MidiTrack() : _ticks(0), _event_index(0)
{
}


MidiTrack::~MidiTrack()
{
	for (MidiEvent* event : _events)
		delete event;

	_events.clear();
}

void MidiTrack::Update(signed int delta_ticks, bool silent)
{
	_ticks += delta_ticks;

	if (_ticks < 0)
	{
		while (_event_index > 0 && -1 * _ticks > _events[_event_index]->delta_ticks)
		{
			_ticks += _events[_event_index]->delta_ticks;

			--_event_index;
		}
	}
	else
	{
		while (_event_index + 1 < _events.size() && _ticks >= _events[_event_index + 1]->delta_ticks)
		{
			_event_index++;

			_ticks -= _events[_event_index]->delta_ticks;

			if (_callback && (!silent || _events[_event_index]->GetMessageUpper() != Events::Midi::NoteOn))
				_callback(_callback_owner, _events[_event_index]);
		}
	}
}

void MidiTrack::LoadFromBuffer(const unsigned char* buffer, unsigned int& pos)
{
	_length = MidiFileUtils::ReadInt(buffer, pos);
	
	unsigned int end = pos + _length;

	while (pos < end)
	{
		_events.push_back(MidiEvent::LoadEvent(buffer, pos));
	}
}

using namespace std;

const string MidiTrack::GetDisplayString() const
{
	string str = "Track:\tLength:" + to_string(_length) + '\n';

	for (MidiEvent* event : _events)
	{
		str += event->GetDisplayString() + '\n';
	}

	return str;
}
