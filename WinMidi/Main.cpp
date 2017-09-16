#include <Windows.h>

#include "WinMidi.h"

LPSTR GetArg1(LPSTR str)
{
	int length = 0;
	bool inStr = false;
	bool isStr = false;

	for (int i = 0; str[i] != '\0'; ++i)
	{
		if (!inStr && str[i] == ' ')
			break;
		else if (str[i] == '\"')
		{
			isStr = true;
			inStr = !inStr;
		}
		else
			length++;
	}

	if (length == 0)
		return NULL;

	LPSTR ret = new char[length + 1];
	ret[length] = '\0';

	int offset = isStr ? 1 : 0;
	for (int i = 0; i < length; ++i)
		ret[i] = str[i + offset];

	return ret;
}

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prev, LPSTR cmd_str, int cmd_show)
{
	WinMidi process;

	LPSTR arg1 = GetArg1(cmd_str);
	if (arg1)
	{
		process.LoadMIDIFile(arg1, true);
		delete[] arg1;
	}

	process.Initialise(instance);
	return process.Run(cmd_show);
}
