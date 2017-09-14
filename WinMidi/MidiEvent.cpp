#include "MidiEvent.h"

#include "MidiUtils.h"
#include "MidiFileUtils.h"

MidiEvent* MidiEvent::LoadEvent(const unsigned char* buffer, unsigned int& pos)
{
	MidiEvent* event = new MidiEvent();
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

void MidiEvent::LoadData(const unsigned char* buffer, unsigned int& pos, unsigned char length)
{
	if (length == 0)length = buffer[pos++];

	SetData(&buffer[pos], length);

	pos += length;
}

void MidiEvent::SetData(const unsigned char* data, unsigned char length)
{
	_data_length = length;

	if (_data) delete[] _data;
	_data = new unsigned char[_data_length];

	for (int i = 0; i < _data_length; i++)
		_data[i] = data[i];
}

DWORD MidiEvent::ToDWORD() const
{
	DWORD word = message;

	if (_data_length > 0)
		word |= (_data[0] << 8);
	if (_data_length > 1)
		word |= (_data[1] << 16);

	return word;
}
