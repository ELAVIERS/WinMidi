#include "MidiPlayer.h"

#include "Error.h"
#include "Midi.h"
#include "MidiEvent.h"
#include "MidiFile.h"

MidiPlayer::MidiPlayer()
{
	HRESULT result = ::midiOutOpen(&_midi_out, MIDI_MAPPER, NULL, 0, CALLBACK_NULL);
	if (result != MMSYSERR_NOERROR)
		Error::ErrorMessage("Could not open MIDI mapper");
}


MidiPlayer::~MidiPlayer()
{
	::midiOutClose(_midi_out);
}

void MidiPlayer::Stop()
{
	Pause();
	_tick = 0;
	_file->ResetTracks();
}

void MidiPlayer::SetFile(MidiFile* file)
{
	_file = file;
	_file->SetCallback(this, _Callback);

	_ticks_per_crotchet = _file->GetDivision();
	_seconds_per_tick = 0.5 / (double)_ticks_per_crotchet; //0.5 spc / tpc = 120bpm
}

void MidiPlayer::Update(double delta_seconds)
{
	if (!_playing)
		return;

	static double	elapsed_time = 0.0;
	static int		ticks = 0;

	elapsed_time += delta_seconds;

	if (elapsed_time >= _seconds_per_tick)
	{
		ticks = (int)(elapsed_time / _seconds_per_tick);
		_file->Update(ticks);
		_tick += ticks;

		elapsed_time -= _seconds_per_tick * ticks;
	}
}

void MidiPlayer::_Callback(void* owner, const MidiEvent* event)
{
	((MidiPlayer*)owner)->_HandleEvent(event);
}

void MidiPlayer::_HandleEvent(const MidiEvent* event)
{
	if (_file->GetDivision() & 0x8000)
		Error::ErrorMessage("SMPTE timecodes not supported yet mate");

	if (event->type == META)
	{
		switch (event->message)
		{
		case Events::Meta::Tempo:
			const unsigned char* data = event->GetData();
			_seconds_per_tick = ((data[0] << 16) + (data[1] << 8) + data[2]) / 1000000.0 / _ticks_per_crotchet;
			break;
		}

		return;
	}

	HRESULT result = ::midiOutShortMsg(_midi_out, event->ToDWORD());
	if (result != MMSYSERR_NOERROR)
		Error::ErrorMessage("MIDI mapper message playback error");
}
