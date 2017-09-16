#include "MidiTrack.h"

#include "MidiEvent.h"
#include "MidiFileUtils.h"

MidiTrack::MidiTrack() : _ticks(0), _next_event(0)
{
}


MidiTrack::~MidiTrack()
{
	for (MidiEvent* event : _events)
		delete event;

	_events.clear();
}

void MidiTrack::Update(unsigned int delta_ticks)
{
	_ticks += delta_ticks;

	if (_next_event < _events.size() && _ticks >= _events[_next_event]->delta_ticks)
	{
		_ticks -= _events[_next_event]->delta_ticks;

		if (_callback)
			_callback(_callback_owner, _events[_next_event]);

		_next_event++;
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
