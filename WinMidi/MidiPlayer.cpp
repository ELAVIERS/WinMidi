#include "MidiPlayer.h"

#include "Error.h"
#include "Midi.h"
#include "MidiEvent.h"
#include "MidiFile.h"

MidiPlayer::MidiPlayer()
{
}


MidiPlayer::~MidiPlayer()
{
}

void MidiPlayer::SetFile(MidiFile* file)
{
	_file = file;
	
	_file->SetEventCallback(this, Callback);
	_ticks_per_crotchet = _file->GetDivision();

	//temporary : look for tempo at start and handle event
	for (const MidiEvent* event : file->GetTracks()[0]->getEvents())
	{
		if (event->delta_ticks == 0 && event->message == Events::Meta::Tempo)
		{
			HandleEvent(event);
			return;
		}
		if (event->delta_ticks != 0)
		{
			MidiEvent temp;
			temp.type = META;
			temp.message = Events::Meta::Tempo;
			unsigned char data[3] = { 0x07, 0xA1, 0x20 }; //500000 microseconds a crotchet (120 bpm)
			temp.SetData(data, 3);
			HandleEvent(&temp);
			return;
		}
	}
}

void MidiPlayer::Update(double delta_seconds)
{
	if (_seconds_per_tick == 0 || !_playing)
		return;

	static double	elapsed_time = 0.0;
	static int		ticks = 0;

	elapsed_time += delta_seconds;
	
	ticks = elapsed_time / (double)_seconds_per_tick;

	if (elapsed_time >= _seconds_per_tick)
	{
		elapsed_time -= _seconds_per_tick * ticks;
		_tick += ticks;
	}
}

void MidiPlayer::Callback(void* owner, const MidiEvent* event)
{
	((MidiPlayer*)owner)->HandleEvent(event);
}

void MidiPlayer::HandleEvent(const MidiEvent* event)
{
	if (_file->GetDivision() & 0x8000)
		ERROR_MSG("SMPTE timecodes not supported yet mate");

	if (event->type == META)
	{
		switch (event->message)
		{
		case Events::Meta::Tempo:
			const unsigned char* data = event->GetData();
			//const unsigned char data[3] = { 0x0B, 0x71, 0xA7 };
			unsigned short microseconds_per_crotchet = (data[0] << 16) + (data[1] << 8) + data[2];

			_seconds_per_tick = (double)0x0B71A7 / 1000000.0 / _ticks_per_crotchet;

			break;

		}
	}


}
