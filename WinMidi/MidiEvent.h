#pragma once
#include <Windows.h>

#include <string>

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

	EventType				type;

	unsigned int			delta_ticks;
	unsigned char			message;

	inline unsigned char	GetMessageUpper() { return message & 0xF0; }

	static MidiEvent* LoadEvent(const unsigned char* Buffer, unsigned int& Pos);
	void LoadData(const unsigned char* Buffer, unsigned int& Pos);
	void SetData(const unsigned char* Data, unsigned char Length);

	const unsigned char*	GetData() const { return _data; };
	const unsigned char		GetDataLength() const { return _data_length; };
	const std::string		GetMessageName();
	const std::string		GetDisplayString();

	DWORD ToDWORD() const;
private:
	unsigned char*	_data;
	unsigned char	_data_length;

	unsigned char	_BytesForNonMeta();
};
