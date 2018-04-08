#include "WorkerThread.h"

#include "Error.h"
#include "WinMidi.h"

WorkerThread::WorkerThread()
{
}


WorkerThread::~WorkerThread()
{
	if (_thread)
		Error::ErrorMessage("Worker thread was deconstructed before stopping");
}

void WorkerThread::Start(WinMidi *inst)
{
	_thread = CreateThread(NULL, 0, _Run, (LPVOID)inst, 0, NULL);
}

void WorkerThread::WaitForStop()
{
	WaitForSingleObject(_thread, INFINITE);

	CloseHandle(_thread);
	_thread = NULL;
}

//static
DWORD WINAPI WorkerThread::_Run(LPVOID param)
{
	WinMidi *process = (WinMidi*)param;

	while (process->IsRunningThread())
	{
		process->Frame();
	}

	return 0;
}
