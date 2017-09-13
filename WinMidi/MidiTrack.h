#pragma once
#include "MidiEvent.h"

#include <vector>

class MidiTrack
{
public:
	MidiTrack();
	~MidiTrack();

	void LoadFromBuffer(const unsigned char* buffer, unsigned int& pos);
	void Update(unsigned int DeltaTicks);
	inline void Reset() { _ticks = _next_event = 0; }

	inline const std::vector<MidiEvent*>& getEvents() const { return _events; };
	const std::string GetDisplayString() const;

	inline void SetCallback(void* Owner, void(*_Callback)(void*, const MidiEvent*)) { _callback_owner = Owner; _callback = _Callback; };
protected:
	std::vector<MidiEvent*> _events;

	unsigned int			_length;

	////Event handling
	unsigned int			_ticks;			//The amount of ticks elapsed
	unsigned int			_next_event;	//Index to the next event to be played

	void*					_callback_owner;
	void (*_callback)(void*, const MidiEvent*);
};

