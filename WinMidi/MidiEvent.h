#pragma once
#include <Windows.h>

enum EventType
{
	MIDI,
	SYSTEM,
	META
};

class MidiEvent
{
public:
	MidiEvent() {};
	~MidiEvent() { if (_data) delete[] _data; };

	EventType		type;

	unsigned int	delta_ticks;
	unsigned char	message;

	static MidiEvent* LoadEvent(const unsigned char* Buffer, unsigned int& Pos);
	void LoadData(const unsigned char* Buffer, unsigned int& Pos, unsigned char Length);
	void SetData(const unsigned char* Data, unsigned char Length);

	const unsigned char* GetData() const { return _data; };
	const unsigned char  GetDataLength() const { return _data_length; };

	DWORD ToDWORD() const;
protected:
	unsigned char*	_data;
	unsigned char	_data_length;
};
