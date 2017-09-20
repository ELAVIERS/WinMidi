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
	void Update(signed int DeltaTicks, bool Silent);
	void SetCallback(void* Owner, void(*Callback)(void*, const MidiEvent*));

	//Getters
	inline unsigned short GetFormat() const					{ return _format; };
	inline const std::vector<MidiTrack*>& GetTracks() const { return _tracks; };
	inline unsigned short GetTicksPerCrotchet() const				{ return _ticks_per_crotchet; };

	const std::string GetDisplayString() const;
protected:
	std::vector<MidiTrack*>	_tracks;

	//Header
	unsigned int			_header_length;
	unsigned short			_format;
	unsigned short			_track_count;

	//Timecode
	bool					_use_timecode;
	unsigned short			_ticks_per_crotchet;
	signed char				_timecode_fps;
	unsigned char			_ticks_per_frame;

};

