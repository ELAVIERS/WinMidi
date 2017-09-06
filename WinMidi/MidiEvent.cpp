#include "MidiEvent.h"

#include "MidiUtils.h"
#include "Shared.h"

Event* Event::LoadEvent(const unsigned char* buffer, unsigned int& pos)
{
	Event* event = new Event();
	event->delta_ticks = MidiFileUtils::ReadVariableLengthInt(buffer, pos);

	if (buffer[pos] == 0xFF)
	{
		pos++;

		event->type = META;
		event->message = buffer[pos++]; //Set event and increment
		event->LoadData(buffer, pos, 0);
	}
	else
	{
		static unsigned char message;

		if (buffer[pos] & 0b10000000) //If a new message is present
			message = buffer[pos++]; //Set message and increment

		if ((message & 0xF0) == 0xF0)
			event->type = SYSTEM;
		else event->type = MIDI;

		event->message = message; //Set event message
		event->LoadData(buffer, pos, MidiUtils::GetEventDataLength(event->message));
	}

	return event;
}

void Event::LoadData(const unsigned char* buffer, unsigned int& pos, unsigned char length)
{
	if (length == 0)length = buffer[pos++];

	_data_length = length;
	if (_data) delete[] _data;
	_data = new unsigned char[_data_length];

	for (int i = 0; i < _data_length; i++)
		_data[i] = buffer[pos++];
}
