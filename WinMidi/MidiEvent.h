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
	Event() {};
	~Event() { if (_data) delete[] _data; };

	EventType type;

	unsigned int delta_ticks;
	unsigned char message;

	void LoadData(const unsigned char* buffer, unsigned int& pos, unsigned char length);
	//if length is 0, length will be determined by indexed byte
	const unsigned char* GetData() { return _data; };
	const unsigned char  GetDataLength() { return _data_length; };

	static Event* LoadEvent(const unsigned char* buffer, unsigned int& pos);
protected:
	unsigned char* _data;
	unsigned char _data_length;
};
