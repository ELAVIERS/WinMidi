#include <Windows.h>

#include "WinMidi.h"

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prev, LPSTR cmd_str, int cmd_show)
{
	WinMidi process;
	process.Initialise(instance);

	return process.Run(cmd_show);
}
