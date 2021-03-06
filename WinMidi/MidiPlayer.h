#pragma once
#include <Windows.h>

class MidiEvent;
class MidiFile;

class MidiPlayer
{
public:
	MidiPlayer();
	~MidiPlayer();

	inline void Play()	{ _playing = true; };
	inline void Pause() { _playing = false; ResetNotes(); };
	inline void Toggle() { if (_playing)Pause(); else Play(); };
	void Stop();

	inline void ResetNotes() { midiOutReset(_midi_out); };

	void Update(double DeltaSeconds);
	void UpdateTicks(signed int DeltaTicks);
	void Seek(double Seconds);

	void SetFile(MidiFile* File);

	inline unsigned int GetCurrentTick() { return _tick; };
	inline bool			IsPlaying() { return _playing; };
	inline bool			IsPlayable() { return _file != NULL; };
protected:
	void _HandleEvent(const MidiEvent* MidiEvent);
	static void _Callback(void*, const MidiEvent*);

	////////////////Player
	bool			_playing;
	unsigned int	_tick;
	double			_seconds_per_tick;
	unsigned int	_tempo;

	HMIDIOUT		_midi_out;

	double			_elapsed_time;
	////////////////File
	MidiFile*		_file;

	unsigned short	_ticks_per_crotchet;
};

