#pragma once
#include "MidiEvent.h"

#include <string>
#include <vector>

class MidiTrack
{
public:
	MidiTrack();
	~MidiTrack();

	void LoadFromBuffer(const unsigned char* buffer, unsigned int& pos);
	void Update(signed int DeltaTicks);
	inline void Reset() { _ticks = _event_index = 0; }

	inline const std::vector<MidiEvent*>& getEvents() const { return _events; };
	const std::string GetDisplayString() const;

	inline void SetCallback(void* Owner, void(*_Callback)(void*, const MidiEvent*)) { _callback_owner = Owner; _callback = _Callback; };

	std::string PushToVector(std::vector<unsigned char> &vec) const;
protected:
	std::vector<MidiEvent*> _events;

	unsigned int			_length;

	////Event handling
	signed int				_ticks;			//The amount of ticks elapsed
	unsigned int			_event_index;	//Index to the next event to be played

	void*					_callback_owner;
	void (*_callback)(void*, const MidiEvent*);
};

