#include "MidiTrack.h"

#include "MidiEvent.h"
#include "MidiFileUtils.h"
#include "MidiUtils.h"

MidiTrack::MidiTrack()
{
}


MidiTrack::~MidiTrack()
{
	for (MidiEvent* event : _events)
		delete event;

	_events.clear();
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
		str += MidiUtils::CreateDisplayString(event) + '\n';
	}

	return str;
}
