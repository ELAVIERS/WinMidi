#pragma once
#include "MidiEvent.h"

#include <vector>

class MidiTrack
{
public:
	MidiTrack();
	~MidiTrack();

	void LoadFromBuffer(const unsigned char* buffer, unsigned int& pos);

	const std::vector<MidiEvent*>& getEvents() const { return _events; };
	const std::string GetDisplayString() const;
protected:
	std::vector<MidiEvent*> _events;

	unsigned int _length;
};

