#pragma once

enum EventType
{
	MIDI,
	SYSTEM,
	META
};

class Event
{
public:
	Event(EventType type) { this->type = type; };
	virtual ~Event() {};

	EventType type;
	unsigned char message;
	unsigned int delta_time;

	static Event* load_event(const unsigned char* buffer, unsigned int& pos);
};

class MidiEvent : public Event
{
public:
	MidiEvent() : Event(MIDI) {};
	virtual ~MidiEvent() {};

	unsigned char data1;
	unsigned char data2;
};

class MetaEvent : public Event
{
public:
	MetaEvent() : Event(META) {};
	virtual ~MetaEvent() { if (_data) delete[] _data; };

	void setLength(unsigned char length);
	unsigned char getLength() { return _length; };

	void setData(const unsigned char* start);
	const unsigned char* getData() { return _data; };
	
protected:
	unsigned char _length;
	unsigned char* _data;
};