#pragma once
#include <string>

class MidiEvent;

namespace MidiUtils {

	const std::string GetEventName(MidiEvent* event);
	const std::string CreateDisplayString(MidiEvent* event);

	unsigned char GetEventDataLength(unsigned char message);

};
