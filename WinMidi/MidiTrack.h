#pragma once
#include "MidiEvent.h"

#include <vector>

class MidiTrack
{
public:
	MidiTrack();
	~MidiTrack();

	void load(const unsigned char* buffer, unsigned int& pos);

	const std::string getDisplayString();
protected:
	std::vector<Event*> _events;

	unsigned int _length;
};

