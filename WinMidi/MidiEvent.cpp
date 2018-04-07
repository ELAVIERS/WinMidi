#include "MidiEvent.h"

#include "Midi.h"
#include "MidiVectorUtils.h"
#include "MidiFileUtils.h"

MidiEvent* MidiEvent::LoadEvent(const unsigned char* buffer, unsigned int& pos)
{
	MidiEvent* event = new MidiEvent();
	event->delta_ticks = MidiFileUtils::ReadVariableLengthInt(buffer, pos);

	if (buffer[pos] == 0xFF)
	{
		pos++;

		event->type = META;
		event->message = buffer[pos++]; //Set event and increment
	}
	else
	{
		static unsigned char message;

		if (buffer[pos] & 0b10000000) //If a new message is present
			message = buffer[pos++]; //Set message and increment

		if ((message & 0xF0) == 0xF0)
			event->type = SYSTEM;
		else event->type = MIDI;

		event->message = message; //Set event message
	}

	event->LoadData(buffer, pos);
	return event;
}

void MidiEvent::LoadData(const unsigned char* buffer, unsigned int& pos)
{
	int len;

	if (type == META)len = buffer[pos++];
	else {
		len = _BytesForNonMeta();
	}
	SetData(&buffer[pos], len);

	pos += len;
}

void MidiEvent::SetData(const unsigned char* data, unsigned char length)
{
	_data_length = length;

	if (_data) delete[] _data;
	_data = new unsigned char[_data_length];

	for (int i = 0; i < _data_length; i++)
		_data[i] = data[i];
}

DWORD MidiEvent::ToDWORD() const
{
	DWORD word = message;

	if (_data_length > 0)
		word |= (_data[0] << 8);
	if (_data_length > 1)
		word |= (_data[1] << 16);

	return word;
}

using namespace std;

void MidiEvent::PushToVector(vector<unsigned char> &vec, unsigned char lastmessage) const {
	MidiVectorUtils::VariableLengthIntToBuffer(vec, delta_ticks);

	if (type == META) {
		vec.push_back(0xFF);
		vec.push_back(message);
		vec.push_back(_data_length);
	}
	else if (message != lastmessage)
		vec.push_back(message);

	for (unsigned char i = 0; i < _data_length; ++i)
		vec.push_back(_data[i]);
}

inline const string ByteToHex(unsigned char byte)
{
	char buf[3];
	sprintf_s(buf, "%X", byte);
	return (string)buf;
}

const string MidiEvent::GetMessageName()
{
	if (type == META) {
		using namespace Events::Meta;
		switch (message)
		{
		case Sequence:			return "Sequence Number";
		case Text:				return "Text";
		case Copyright:			return "Copyright";
		case TrackName:			return "Track Name";
		case InstrumentName:	return "Instrument Name";
		case Lyric:				return "Lyric";
		case Marker:			return "Marker";
		case Cue:				return "Cue Point";
		case ChannelPrefix:		return "Channel Prefix";
		case MidiPort:			return "MIDI Port";
		case TrackEnd:			return "Track End";
		case Tempo:				return "Set Tempo";
		case TimeCodeOffset:	return "SMPTE Offset";
		case TimeSignature:		return "Time Signature";
		case KeySignature:		return "Key Signature";
		case SequencerCustom:	return "Sequencer-Specific";
		}
	}
	else if (type == SYSTEM) {
		using namespace Events::System;
		switch (message)
		{
		case SystemExclusive:	return "System Exclusive";
		case SongPosition:		return "Song Position Pointer";
		case SongSelect:		return "Song Select";
		case TuneRequest:		return "Tune Request";
		case TimingClock:		return "Timing Clock";
		case Start:				return "Start";
		case Continue:			return "Continue";
		case Stop:				return "Stop";
		case ActiveSensing:		return "Active Sensing";
		}
	}
	else
	{
		using namespace Events::Midi;
		switch (message & 0xF0)
		{
		case NoteOff:			return "Note Off";
		case NoteOn:			return _data[1] == 0 ? "Note Off" : "Note On";
		case PolyAftertouch:	return "Polymorphic Aftertouch";
		case ControlChange:		return "Control Change";
		case ProgramChange:		return "Program Change";
		case ChannelAftertouch:	return "Channel Aftertouch";
		case PitchWheel:		return "Pitch Wheel";
		}
	}

	return "Unknown (0x" + ByteToHex(message) + ")";
}

const string MidiEvent::GetDisplayString()
{
	string pre = "";
	if (delta_ticks != 0)
		pre = '(' + to_string(delta_ticks) + " Tick" + (delta_ticks == 1 ? "" : "s") + ")\n";

	string str = (string)(type == META ? "Meta" : type == SYSTEM ? "System" : "MIDI") + "\t" + GetMessageName();

	if (type == MIDI)
	{
		unsigned char msg = message & 0xF0;

		if (str.length() < 13) str += '\t';//OCD here

		str += "\t\tChannel " + to_string((int)(message & 0x0f));

		using namespace Events::Midi;

		switch (msg)
		{
		case NoteOff:
		case NoteOn:
		case PolyAftertouch:
			str += "\tNote " + to_string(_data[0]) + "\t\tVelocity " + to_string(_data[1]);
			break;
		case ControlChange:
			str += "\tController " + to_string(_data[0]) + "\tValue " + to_string(_data[1]);
			break;
		case ProgramChange:
			str += "\tProgram " + to_string(_data[0]);
			break;
		case ChannelAftertouch:
			str += "\tVelocity " + to_string(_data[0]);
			break;
		}
	}
	else if (type == META)
	{
		if (_data_length > 0)
			str += "\t\t";

		for (unsigned int i = 0; i < _data_length; i++)
			str += (i == 0 ? "" : " ") + (string)ByteToHex(_data[i]);
	}

	return pre + str;
}

unsigned char MidiEvent::_BytesForNonMeta()
{
	if (type == SYSTEM) //If system event
		switch (message)
		{
		case Events::System::SongPosition:
			return 2;
		case Events::System::SongSelect:
			return 1;
		default:
			return 0;
		}

	unsigned char instr = message & 0xF0;
	if (instr == Events::Midi::ProgramChange || instr == Events::Midi::ChannelAftertouch)
		return 1;

	return 2;
}

