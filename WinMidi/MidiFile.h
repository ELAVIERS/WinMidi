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
	void DisplayStringToFile(const char* path);

	//Setters
	inline void SetEventCallback(void* Object, void(*Callback)(void*, const MidiEvent*))
	{
		_callback_owner = Object;
		_callback = Callback;
	};

	//Getters
	inline unsigned short GetFormat() const					{ return _format; };
	inline const std::vector<MidiTrack*>& GetTracks() const { return _tracks; };
	inline unsigned short GetDivision() const				{ return _ticks_per_crotchet; };

	const std::string GetDisplayString() const;
protected:
	std::vector<MidiTrack*> _tracks;
	
	//Callback
	void* _callback_owner;
	void(*_callback)(void*, const MidiEvent*);

	//Header
	unsigned int _header_length;
	unsigned short _format;
	unsigned short _track_count;

	//Timecode
	bool _use_timecode;
	unsigned short _ticks_per_crotchet;
	signed char _timecode_fps;
	unsigned char _ticks_per_frame;

};

