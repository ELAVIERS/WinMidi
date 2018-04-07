#pragma once
#include <vector>

namespace MidiVectorUtils
{
	inline void VariableLengthIntToBuffer(std::vector<unsigned char> &vec, unsigned int value) {
		unsigned int length = 1;

		if (value &			0b11110000000000000000000000000000)
			length = 5;
		else if (value &	0b00001111111000000000000000000000)
			length = 4;
		else if (value &	0b00000000000111111100000000000000)
			length = 3;
		else if (value &	0b00000000000000000011111110000000)
			length = 2;

		for (unsigned int i = length - 1; i > 0; --i)
		{
			vec.push_back(0b10000000 | (value >> (7 * i)) & 0xFF);
		}

		vec.push_back(value & 0b01111111);
	}

	inline void Int16ToVector(std::vector<unsigned char> &vec, __int16 value) {
		vec.push_back((value & 0xFF00) >> 8);
		vec.push_back(value & 0x00FF);
	}

	inline void Int32ToVector(std::vector<unsigned char> &vec, __int32 value) {
		vec.push_back((value & 0xFF000000) >> 24);
		vec.push_back((value & 0x00FF0000) >> 16);
		vec.push_back((value & 0x0000FF00) >> 8);
		vec.push_back(value & 0x000000FF);
	}
}
