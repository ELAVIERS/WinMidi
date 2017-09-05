#pragma once

#include <Windows.h>
#define ERROR_MSG(msg) ::MessageBoxA(0, msg, "Error", MB_OK)
//hmm

namespace MidiFileUtils
{
	inline unsigned int ReadVariableLengthInt(const unsigned char* buffer, unsigned int& pos)
	{
		unsigned int amount = 0;
		unsigned int length = 0;

		while (buffer[pos] & 0b10000000)
		{
			pos++;
			length++;
		}

		for (unsigned int i = 0; i <= length; i++)
		{
			amount += (buffer[pos - i] & 0b01111111) << (7 * i);
		}

		pos++; //Move out of variable length int
		return amount;
	}

	inline bool ReadStringAndCheck(const unsigned char* str_a, unsigned int& pos, unsigned int len, const char* str_b)
	{
		for (unsigned int i = pos; i <= pos + len; i++)
			if (str_a[i] != str_b[i - pos])
				return false;

		pos += len;
		return true;
	}

	inline short ReadShort(const unsigned char* buffer, unsigned int& pos)
	{
		short value = (buffer[pos] << 8) + buffer[pos + 1];

		pos += 2;
		return value;
	}

	inline int ReadInt(const unsigned char* buffer, unsigned int& pos)
	{
		int value = (buffer[pos] << 24) + (buffer[pos + 1] << 16) + (buffer[pos + 2] << 8) + buffer[pos + 3];

		pos += 4;
		return value;
	}
}
