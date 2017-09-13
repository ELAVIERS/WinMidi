#pragma once

class MidiEvent;
class MidiFile;

class MidiPlayer
{
public:
	MidiPlayer();
	~MidiPlayer();

	inline void Play()	{ _playing = true; };
	inline void Pause() { _playing = false; };
	void Stop();

	void Update(double DeltaSeconds);

	void SetFile(MidiFile* File);

	unsigned int GetCurrentTick() { return _tick; };
protected:
	void _HandleEvent(const MidiEvent* MidiEvent);
	static void _Callback(void*, const MidiEvent*);

	////////////////Player
	bool			_playing;
	unsigned int	_tick;
	double			_seconds_per_tick;

	////////////////File
	MidiFile*		_file;

	int				_ticks_per_crotchet;
};

