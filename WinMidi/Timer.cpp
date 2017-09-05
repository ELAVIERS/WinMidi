#include "Timer.h"
#include <Windows.h>
#include "Shared.h"

Timer::Timer()
{
	if (!QueryPerformanceFrequency(&_freq))
		ERROR_MSG("We're not in 1998; I'm not supporting your bad timer.");
}


Timer::~Timer()
{
}

void Timer::start()
{
	::QueryPerformanceCounter(&_start_time);
}

float Timer::stop()
{
	static LARGE_INTEGER end_time;
	::QueryPerformanceCounter(&end_time);

	return ((float)end_time.QuadPart - (float)_start_time.QuadPart) / (float)_freq.QuadPart;
}
