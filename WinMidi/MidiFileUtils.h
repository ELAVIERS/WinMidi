#pragma once

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

	inline __int16 ReadInt16(const unsigned char* buffer, unsigned int& pos)
	{
		__int16 value = (buffer[pos] << 8) + buffer[pos + 1];

		pos += 2;
		return value;
	}

	inline __int32 ReadInt32(const unsigned char* buffer, unsigned int& pos)
	{
		__int32 value = (buffer[pos] << 24) + (buffer[pos + 1] << 16) + (buffer[pos + 2] << 8) + buffer[pos + 3];

		pos += 4;
		return value;
	}
}
