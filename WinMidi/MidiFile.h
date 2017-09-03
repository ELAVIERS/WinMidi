#pragma once
#include "MidiTrack.h"

#include <string>
#include <vector>
#include <Windows.h>

class MidiFile
{
public:
	MidiFile();
	~MidiFile();

	void loadFromDirectory(HWND owner);

	const std::string getDisplayString();
protected:
	std::vector<MidiTrack*> _tracks;

	unsigned int header_len;			//4 BYTES
	unsigned short format;				//2 BYTES
	unsigned short track_chunk_count;	//2 BYTES
	
	bool useSMPTE;

	unsigned short ticks_per_beat;

	signed char smpte_fps;
	unsigned char smpte_ticks_per_frame;

};

