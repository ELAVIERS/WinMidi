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

	bool LoadFromFile(const char* File);
	void DisplayStringToFile(const char* path);

	//Tracks
	void ResetTracks();
	void Update(signed int DeltaTicks);
	void SetCallback(void* Owner, void(*Callback)(void*, const MidiEvent*));

	//Getters
	inline unsigned short GetFormat() const					{ return _format; };
	inline const std::vector<MidiTrack*>& GetTracks() const { return _tracks; };
	inline unsigned short GetTicksPerCrotchet() const				{ return _ticks_per_crotchet; };

	const std::string GetDisplayString() const;

	std::string PushToVector(std::vector<unsigned char> &vec, int track);
protected:
	std::vector<MidiTrack*>	_tracks;

	//Header
	unsigned int			_header_length;
	unsigned short			_format;
public:
	unsigned short			track_count;
protected:
	unsigned short			_division;

	//Timecode
	bool					_use_timecode;
	unsigned short			_ticks_per_crotchet;
	signed char				_timecode_fps;
	unsigned char			_ticks_per_frame;

};

