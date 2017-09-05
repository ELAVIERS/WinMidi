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

	void LoadFromDirectory(HWND owner);

	//Getters
	unsigned short GetFormat() const { return _format; };
	const std::vector<MidiTrack*>& GetTracks() const { return _tracks; };
	const std::string GetDisplayString() const;
protected:
	std::vector<MidiTrack*> _tracks;

	unsigned int _header_length;			//4 BYTES
	unsigned short _format;				//2 BYTES
	unsigned short _track_count;	//2 BYTES
	
	bool _use_timecode;

	unsigned short _ticks_per_crotchet;

	signed char _timecode_fps;
	unsigned char _ticks_per_frame;

};

