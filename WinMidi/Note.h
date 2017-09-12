#pragma once
#include <vector>

class MidiEvent;

class Note
{
public:
	Note();
	~Note();

	void render() {};

	unsigned int Start, end;
	unsigned char pitch;
};

