#pragma once
#include <string>

class Event;

namespace MidiUtils {

	const std::string GetEventName(Event* event);
	const std::string CreateDisplayString(Event* event);

	unsigned char GetEventDataLength(unsigned char message);

};
