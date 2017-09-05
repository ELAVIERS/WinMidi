#include "MidiUtils.h"
#include "Midi.h"
#include "MidiEvent.h"
#include "Shared.h"

using namespace std;

inline string ToHex(unsigned char num) //yuck
{
	char buf[3];
	sprintf_s(buf, "%X", num);
	return (string)buf;
}

const string MidiUtils::GetEventName(Event* event)
{
	if (event->type == META) {
		using namespace Events::Meta;
		switch (event->message)
		{
		case Sequence:
			return "Sequence Number";
		case Text:
			return "Text";
		case Copyright:
			return "Copyright";
		case TrackName:
			return "Track Name";
		case InstrumentName:
			return "Instrument Name";
		case Lyric:
			return "Lyric";
		case Marker:
			return "Marker";
		case Cue:
			return "Cue Point";
		case ChannelPrefix:
			return "Channel Prefix";
		case MidiPort:
			return "MIDI Port";
		case TrackEnd:
			return "Track End";
		case Tempo:
			return "Set Tempo";
		case TimeCodeOffset:
			return "SMPTE Offset";
		case TimeSignature:
			return "Time Signature";
		case KeySignature:
			return "Key Signature";
		case SequencerCustom:
			return "Sequencer-Specific";
		}
	}
	else if (event->type == SYSTEM) {
		using namespace Events::System;
		switch (event->message)
		{
		case SystemExclusive:
			return "System Exclusive";
		case SongPosition:
			return "Song Position Pointer";
		case SongSelect:
			return "Song Select";
		case TuneRequest:
			return "Tune Request";
		case TimingClock:
			return "Timing Clock";
		case Start:
			return "Start";
		case Continue:
			return "Continue";
		case Stop:
			return "Stop";
		case ActiveSensing:
			return "Active Sensing";
		}
	}
	else
	{
		using namespace Events::Midi;
		switch (event->message & 0xF0)
		{
		case NoteOff:
			return "Note Off";
		case NoteOn:
			return event->GetData()[1] == 0 ? "Note Off" : "Note On";
		case PolyAftertouch:
			return "Polymorphic Aftertouch";
		case ControlChange:
			return "Control Change";
		case ProgramChange:
			return "Program Change";
		case ChannelAftertouch:
			return "Channel Aftertouch";
		case PitchWheel:
			return "Pitch Wheel";
		}
	}

	return "Unknown (0x" + ToHex(event->message) + ")";
}

const string MidiUtils::CreateDisplayString(Event* event)
{
	string pre = "";
	if (event->delta_ticks)
		pre = '(' + to_string(event->delta_ticks) + " Tick" + (event->delta_ticks == 1 ? "" : "s") + ")\n";

	string str = (string)(event->type == META ? "Meta" : event->type == SYSTEM ? "System" : "MIDI") + "\t" + GetEventName(event);

	unsigned char data_count = GetEventDataLength(event->message);

	if (event->type == MIDI)
	{
		unsigned char msg = event->message & 0xF0;

		if (str.length() < 13) str += '\t';//OCD here

		str += "\t\tChannel " + to_string((int)(event->message & 0x0f));

		using namespace Events::Midi;

		switch (msg)
		{
		case NoteOff:
		case NoteOn:
		case PolyAftertouch:
			str += "\tNote " + to_string(event->GetData()[0]) + "\t\tVelocity " + to_string(event->GetData()[1]);
			break;
		case ControlChange:
			str += "\tController " + to_string(event->GetData()[0]) + "\tValue " + to_string(event->GetData()[1]);
			break;
		case ProgramChange:
			str += "\tProgram " + to_string(event->GetData()[0]);
			break;
		case ChannelAftertouch:
			str += "\tVelocity " + to_string(event->GetData()[0]);
			break;
		}
	}
	else if (event->type == META)
	{
		unsigned int len = event->GetDataLength();
		const unsigned char* data = event->GetData();

		if (len > 0)
			str += "\t\t";

		for (unsigned int i = 0; i < len; i++)
			str += (i == 0 ? "" : " ") + ToHex(data[i]);
	}

	return pre + str;
}

unsigned char MidiUtils::GetEventDataLength(unsigned char event)
{
	unsigned char instr = event & 0xF0;

	if (instr == 0xF0) //If system event
	{
		if (event == Events::System::SystemExclusive || event == Events::System::SystemExclusiveEnd)
			ERROR_MSG("yo dood don't call get_event_data_count on a sysex event srsly y u do dis");

		switch (event)
		{
		case Events::System::SongPosition:
			return 2;
		case Events::System::SongSelect:
			return 1;
		}

		return 0;
	}

	if (instr == Events::Midi::ProgramChange || instr == Events::Midi::ChannelAftertouch)
		return 1;

	return 2;
}
