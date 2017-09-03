#pragma once
#include <string>

class Event;

namespace MidiUtils {

	const std::string getEventName(Event* event);

	const std::string createDisplayString(Event* event);

	unsigned char getEventDataLength(unsigned char message);

};
