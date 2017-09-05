#pragma once
#include <vector>

class MidiFile;

class Note
{
public:
	Note();
	~Note();

	void render() {};

	unsigned int start, end;
	unsigned char pitch;

	static const std::vector<Note> GetNotes(const MidiFile& file);
};

