#include "MidiTrack.h"

#include "Midi.h"
#include "MidiEvent.h"
#include "MidiFileUtils.h"
#include "MidiVectorUtils.h"

MidiTrack::MidiTrack() : _ticks(0), _event_index(0)
{
}


MidiTrack::~MidiTrack()
{
	for (MidiEvent* event : _events)
		delete event;

	_events.clear();
}

void MidiTrack::Update(signed int delta_ticks)
{
	_ticks += delta_ticks;

	if (_ticks < 0)
	{
		while (_event_index > 0 && _ticks < 0)
		{
			--_event_index;

			_ticks += _events[_event_index]->delta_ticks;
		}
	}
	else
	{
		while (_event_index < _events.size() && (unsigned int)_ticks >= _events[_event_index]->delta_ticks)
		{
			_ticks -= _events[_event_index]->delta_ticks;

			if (_callback)
				_callback(_callback_owner, _events[_event_index]);

			++_event_index;
		}
	}
}

void MidiTrack::LoadFromBuffer(const unsigned char* buffer, unsigned int& pos)
{
	_length = MidiFileUtils::ReadInt32(buffer, pos);
	
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

std::string MidiTrack::PushToVector(vector<unsigned char> &vec) const {
	vec.push_back('M'); vec.push_back('T'); vec.push_back('r'); vec.push_back('k');

	MidiVectorUtils::Int32ToVector(vec, _length);

	std::string name;

	unsigned char lastmessage = 0;
	for (MidiEvent* event : _events)
	{
		event->PushToVector(vec, lastmessage);
		lastmessage = event->message;

		if (event->message == Events::Meta::TrackName)
		{
			for (unsigned char i = 0; i < event->GetDataLength(); ++i)
				name.push_back(event->GetData()[i]);
		}
	}

	return name;
}
