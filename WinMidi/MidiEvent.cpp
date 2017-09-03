#include "MidiEvent.h"
#include "Shared.h"
#include "MidiUtils.h"

Event* Event::load_event(const unsigned char* buffer, unsigned int& pos)
{
	unsigned int delta_time = get_var_length_big(buffer, pos);

	if (buffer[pos] == 0xFF)
	{
		pos++;

		MetaEvent* event = new MetaEvent();
		event->delta_time = delta_time;

		event->message = buffer[pos++]; //Set event and increment
		event->setLength(buffer[pos++]); //Set length and increment
		event->setData(&buffer[pos]); //Set data to load from current position

		pos += event->getLength(); //Move out of data

		return event;
	}
	else
	{
		MidiEvent* event = new MidiEvent();
		static unsigned char message;

		event->delta_time = delta_time;

		if (buffer[pos] & 0b10000000) //If a new message is present
			message = buffer[pos++]; //Set message and increment

		if ((message & 0xF0) == 0xF0)
			event->type = SYSTEM;
		else event->type = MIDI;

		event->message = message; //Set event message

		unsigned char length = MidiUtils::getEventDataLength(event->message);

		if (length > 0)
		{
			event->data1 = buffer[pos++];
			if (length > 1)
				event->data2 = buffer[pos++];
		}

		return event;
	}
}

void MetaEvent::setLength(unsigned char length)
{
	_length = length;
	if (_data) delete[] _data;
	_data = new unsigned char[length];
}

void MetaEvent::setData(const unsigned char* start)
{
	for (int i = 0; i < _length; i++)
		_data[i] = start[i];
}
