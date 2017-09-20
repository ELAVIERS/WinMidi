#include "MidiFile.h"

#include "Error.h"
#include "MidiFileUtils.h"

#include <fstream>

MidiFile::MidiFile()
{
}


MidiFile::~MidiFile()
{
}

using namespace std;
using namespace Error;

bool MidiFile::LoadFromFile(const char* path)
{
	ifstream file;
	file.open(path, ios::in | ios::binary);

	if (!file.is_open()) {
		ErrorMessage("Could not open");
		return false;
	}

	for (MidiTrack* track : _tracks)
		delete track;
	_tracks.clear();

	file.seekg(0, file.end);
	unsigned int buffer_len = (unsigned int)file.tellg() + 1;
	file.seekg(0, file.beg);

	unsigned char *buffer = new unsigned char[buffer_len];
	file.read((char*)buffer, buffer_len);
	file.close();

	using namespace MidiFileUtils;
	unsigned int pos = 0;

	if (!ReadStringAndCheck(buffer, pos, 4, "MThd"))
	{
		ErrorMessage("Invalid MIDI");
		return false;
	}

	_header_length = ReadInt(buffer, pos);
	if (_header_length != 6)
	{
		ErrorMessage("U WOT (header length is not 6)");
		return false;
	}

	_format = ReadShort(buffer, pos);
	_track_count = ReadShort(buffer, pos);
	short division = ReadShort(buffer, pos);

	_use_timecode = division & 0x8000;

	if (_use_timecode)
	{						   //0b0100 0000 0000 0000
		_timecode_fps = ((division & 0x4000) ? -64 : 0) + ((division & 0x3F00) >> 8); //0b0011 1111 0000 0000
		_ticks_per_frame = division & 0x00FF;
	}
	else
		_ticks_per_crotchet = division;

	while (ReadStringAndCheck(buffer, pos, 4, "MTrk"))
	{
		MidiTrack* trk = new MidiTrack;
		trk->LoadFromBuffer(buffer, pos);

		_tracks.push_back(trk);
	}
	
	return true;
}

const string MidiFile::GetDisplayString() const
{
	string str = "Header:\tFormat " + to_string(_format) + "\t\t" + to_string(_track_count) + " Chunks\t";

	if (_use_timecode) str += to_string(_timecode_fps) + " FPS\t\t" + to_string(_ticks_per_frame) + " Ticks per frame\n";
	else str += to_string(_ticks_per_crotchet) + " Ticks per beat\n";

	for (MidiTrack* track : _tracks)
		str += track->GetDisplayString() + "\n\n";

	return str;
}

void MidiFile::ResetTracks()
{
	for (auto track = _tracks.begin(); track != _tracks.end(); ++track)
		(*track)->Reset();
}

void MidiFile::Update(signed int delta_ticks, bool silent)
{
	for (auto track = _tracks.begin(); track != _tracks.end(); ++track)
		(*track)->Update(delta_ticks, silent);
}

void MidiFile::SetCallback(void* owner, void(*callback)(void*, const MidiEvent*))
{
	for (auto track = _tracks.begin(); track != _tracks.end(); ++track)
		(*track)->SetCallback(owner, callback);
}

void MidiFile::DisplayStringToFile(const char* path)
{
	ofstream file(path);

	file << GetDisplayString();

	file.close();
}
