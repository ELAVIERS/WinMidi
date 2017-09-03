#include "MidiTrack.h"
#include "Shared.h"
#include "MidiEvent.h"
#include "MidiUtils.h"

MidiTrack::MidiTrack()
{
}


MidiTrack::~MidiTrack()
{
	for (Event* event : _events)
		delete event;

	_events.clear();
}

void MidiTrack::load(const unsigned char* buffer, unsigned int& pos)
{
	_length = get_int_big(buffer, pos);

	unsigned int end = pos + _length;

	while (pos < end)
	{
		_events.push_back(Event::load_event(buffer, pos));
	}
}

using namespace std;

const string MidiTrack::getDisplayString()
{
	string str = "Track:\tLength:" + to_string(_length) + '\n';

	for (Event* event : _events)
	{
		str += MidiUtils::createDisplayString(event) + '\n';
	}

	return str;
}
