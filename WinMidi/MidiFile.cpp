#include "MidiFile.h"

#include "Shared.h"

#include <fstream>
#include <shobjidl.h>

MidiFile::MidiFile()
{
}


MidiFile::~MidiFile()
{
}

void MidiFile::LoadFromDirectory(HWND owner)
{
	IFileOpenDialog* open_dialog;
	COMDLG_FILTERSPEC filters[] = { { L"MIDI Files", L"*.MID;*.MIDI" },{ L"All Files", L"*.*" } };

	HRESULT result = CoCreateInstance(
		CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&open_dialog));

	if (SUCCEEDED(result)) {
		using namespace std;
		using namespace MidiFileUtils;

		open_dialog->SetFileTypes(2, filters);
		open_dialog->Show(owner);

		LPWSTR file_name;
		IShellItem* item;

		result = open_dialog->GetResult(&item);
		if (!SUCCEEDED(result))
			return;

		item->GetDisplayName(SIGDN_FILESYSPATH, &file_name);

		ifstream file;
		file.open(file_name, ios::in | ios::binary);

		if (!file.is_open()) {
			ERROR_MSG("Could not open");
			return;
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

		unsigned int pos = 0;

		if (!ReadStringAndCheck(buffer, pos, 4, "MThd"))
		{
			ERROR_MSG("Invalid MIDI");
			return;
		}

		_header_length = ReadInt(buffer, pos);
		if (_header_length != 6)
		{
			ERROR_MSG("U WOT (header length is not 6)");
			return;
		}

		_format =			ReadShort(buffer, pos);
		_track_count =		ReadShort(buffer, pos);
		short division =	ReadShort(buffer, pos);

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
	}
	else 
		ERROR_MSG("Could not open file dialog");
}

using namespace std;

const string MidiFile::GetDisplayString() const
{
	string str = "Header:\tFormat " + to_string(_format) + "\t\t" + to_string(_track_count) + " Chunks\t";

	if (_use_timecode) str += to_string(_timecode_fps) + " FPS\t\t" + to_string(_ticks_per_frame) + " Ticks per frame\n";
	else str += to_string(_ticks_per_crotchet) + " Ticks per beat\n";

	for (MidiTrack* track : _tracks)
		str += track->GetDisplayString() + "\n\n";

	return str;
}

void MidiFile::DisplayStringToFile(const char* path)
{
	ofstream file(path);

	file << GetDisplayString();

	file.close();
}
