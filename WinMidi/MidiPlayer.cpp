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
	_elapsed_time = 0.0;
	_file->ResetTracks();
}

void MidiPlayer::SetFile(MidiFile* file)
{
	_file = file;
	_file->SetCallback(this, _Callback);
	Stop();

	_ticks_per_crotchet = _file->GetTicksPerCrotchet();
	_seconds_per_tick = 0.5 / (double)_ticks_per_crotchet; //0.5 spc / tpc = 120bpm
}

void MidiPlayer::UpdateTicks(signed int ticks)
{
	if (ticks < 0) {
		ResetNotes();
		_file->Update(ticks);
	}
	else
		_file->Update(ticks);

	_tick += ticks;
}

void MidiPlayer::Update(double delta_seconds)
{
	if (!_playing || !_file)
		return;

	_elapsed_time += delta_seconds;

	if (abs(_elapsed_time) >= _seconds_per_tick)
	{
		static signed int	ticks;
		ticks = (signed int)(_elapsed_time / _seconds_per_tick);
		
		UpdateTicks(ticks);

		_elapsed_time -= _seconds_per_tick * ticks;
	}
}

void MidiPlayer::Seek(double seconds)
{
	ResetNotes();

	signed int ticks = (signed int)(seconds / _seconds_per_tick);
	_file->Update(ticks);
	_tick += ticks;
}

void MidiPlayer::_Callback(void* owner, const MidiEvent* event)
{
	((MidiPlayer*)owner)->_HandleEvent(event);
}

void MidiPlayer::_HandleEvent(const MidiEvent* event)
{
	if (event->type == META)
	{
		switch (event->message)
		{
		case Events::Meta::Tempo:
			const unsigned char* data = event->GetData();
			_tempo = (data[0] << 16) + (data[1] << 8) + data[2];
			
			_seconds_per_tick = _tempo / 1000000.0 / _ticks_per_crotchet;
			break;
		}

		return;
	}

	::midiOutShortMsg(_midi_out, event->ToDWORD());
}
