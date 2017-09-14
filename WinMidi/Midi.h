#pragma once

namespace Events
{
	namespace Midi {
		typedef enum {
			NoteOff =			0x80,//0b1000nnnn
			NoteOn =			0x90,//0b1001nnnn
			PolyAftertouch =	0xA0,//0b1010nnnn
			ControlChange =		0xB0,//0b1011nnnn
			ProgramChange =		0xC0,//0b1100nnnn
			ChannelAftertouch =	0xD0,//0b1101nnnn
			PitchWheel =		0xE0 //0b1110nnnn
		} MidiMessage;
	}

	namespace System {
		typedef enum {
			SystemExclusive =	0xF0,
			QuarterFrame =		0xF1,
			SongPosition =		0xF2,
			SongSelect =		0xF3,
			TuneRequest =		0xF6,
			SystemExclusiveEnd =0xF7,
			TimingClock =		0xF8,
			Start =				0xFA,
			Continue =			0xFB,
			Stop =				0xFC,
			ActiveSensing =		0xFE
		} SystemMessage;
	}

	namespace Meta {
		typedef enum {
			Sequence =			0x00,
			Text =				0x01,
			Copyright =			0x02,
			TrackName =			0x03,
			InstrumentName =	0x04,
			Lyric =				0x05,
			Cue =				0x06,
			Marker =			0x07,
			ChannelPrefix =		0x20,
			MidiPort =			0x21,
			TrackEnd =			0x2F,
			Tempo =				0x51,
			TimeCodeOffset =	0x54,
			TimeSignature =		0x58,
			KeySignature =		0x59,
			SequencerCustom =	0x7F
		} MetaMessage;
	}
}
