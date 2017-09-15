#include <Windows.h>

#include "WinMidi.h"

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prev, LPSTR cmd_str, int cmd_show)
{
	WinMidi process;
	if (*cmd_str)
		process.LoadMIDIFile(cmd_str);

	process.Initialise(instance);
	return process.Run(cmd_show);
}
