#pragma once
#include <Windows.h>

class WinMidi;

class WorkerThread
{
private:
	HANDLE _thread;

	static DWORD WINAPI _Run(LPVOID);
public:
	WorkerThread();
	~WorkerThread();

	void Start(WinMidi*);
	void WaitForStop();
};

