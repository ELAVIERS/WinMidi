#include "MidiFile.h"
#include "Shared.h"
#include <shobjidl.h>
#include <fstream>

MidiFile::MidiFile()
{
}


MidiFile::~MidiFile()
{
}

void MidiFile::loadFromDirectory(HWND owner)
{
	IFileOpenDialog* open_dialog;
	COMDLG_FILTERSPEC filters[] = { { L"MIDI Files", L"*.MID;*.MIDI" },{ L"All Files", L"*.*" } };

	HRESULT result = CoCreateInstance(
		CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&open_dialog));

	if (SUCCEEDED(result)) {
		using namespace std;

		open_dialog->SetFileTypes(2, filters);
		open_dialog->Show(owner);

		LPWSTR file_name;
		IShellItem* item;
		open_dialog->GetResult(&item);
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

		unsigned int pos = 0;

		if (!check(buffer, pos, 4, "MThd"))
		{
			ERROR_MSG("Invalid MIDI");
			return;
		}

		header_len = get_int_big(buffer, pos);
		if (header_len != 6)
		{
			ERROR_MSG("U WOT (header_len is not 6)");
			return;
		}

		format =				get_short_big(buffer, pos);
		track_chunk_count =		get_short_big(buffer, pos);
		short division =		get_short_big(buffer, pos);

		useSMPTE = division & 0x8000;

		if (useSMPTE)
		{						   //0b0100 0000 0000 0000
			smpte_fps = ((division & 0x4000) ? -64 : 0) + ((division & 0x3F00) >> 8); //0b0011 1111 0000 0000
			smpte_ticks_per_frame = division & 0x00FF;
		}
		else
			ticks_per_beat = division;

		while (check(buffer, pos, 4, "MTrk"))
		{
			MidiTrack* trk = new MidiTrack;
			trk->load(buffer, pos);

			_tracks.push_back(trk);
		}

		file.close();
	}
	else 
		ERROR_MSG("Could not open file dialog");
}

using namespace std;

const string MidiFile::getDisplayString()
{
	string str = "Header:\tFormat " + to_string(format) + "\t\t" + to_string(track_chunk_count) + " Chunks\t";

	if (useSMPTE) str += to_string(smpte_fps) + " FPS\t\t" + to_string(smpte_ticks_per_frame) + " Ticks per frame\n";
	else str += to_string(ticks_per_beat) + " Ticks per beat\n";

	for (MidiTrack* track : _tracks)
		str += track->getDisplayString() + "\n\n";

	return str;
}
