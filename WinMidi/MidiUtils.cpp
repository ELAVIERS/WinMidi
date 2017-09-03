#include "MidiUtils.h"
#include "Midi.h"
#include "MidiEvent.h"
#include "Shared.h"

using namespace std;

inline string toHex(unsigned char num) //yuck
{
	char buf[3];
	sprintf_s(buf, "%X", num);
	return (string)buf;
}

const string MidiUtils::getEventName(Event* event)
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
			return ((MidiEvent*)event)->data2 == 0 ? "Note Off" : "Note On";
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

	return "Unknown (0x" + toHex(event->message) + ")";
}

const string MidiUtils::createDisplayString(Event* event)
{
	string str = (string)(event->type == META ? "Meta" : event->type == SYSTEM ? "System" : "MIDI") + "\t" + getEventName(event);

	unsigned char data_count = getEventDataLength(event->message);

	if (event->type == MIDI)
	{
		unsigned char msg = event->message & 0xF0;
		MidiEvent* midEvent = (MidiEvent*)event;

		if (str.length() < 13) str += '\t';//OCD here

		str += "\t\tChannel " + to_string((int)(event->message & 0x0f));

		using namespace Events::Midi;

		switch (msg)
		{
		case NoteOff:
		case NoteOn:
		case PolyAftertouch:
			return str += "\tNote " + to_string(midEvent->data1) + "\t\tVelocity " + to_string(midEvent->data2);
		case ControlChange:
			return str += "\tController " + to_string(midEvent->data1) + "\tValue " + to_string(midEvent->data2);
		case ProgramChange:
			return str += "\tProgram " + to_string(midEvent->data1);
		case ChannelAftertouch:
			return str += "\tVelocity " + to_string(midEvent->data1);
		}

		return str;
	}
	else if (event->type == META)
	{
		MetaEvent *metaEvent = (MetaEvent*)event;
		unsigned int len = metaEvent->getLength();
		const unsigned char* data = metaEvent->getData();

		if (len > 0)
			str += "\t\t";

		for (unsigned int i = 0; i < len; i++)
			str += (i == 0 ? "" : " ") + toHex(data[i]);
	}

	return str;
}

unsigned char MidiUtils::getEventDataLength(unsigned char event)
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
