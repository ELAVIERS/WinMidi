#include "Timer.h"

#include "Error.h"

#include <Windows.h>

Timer::Timer()
{
	if (!QueryPerformanceFrequency(&_freq))
		Error::ErrorMessage("We're not in 1998; I'm not supporting your bad timer.");
}


Timer::~Timer()
{
}

void Timer::Start()
{
	::QueryPerformanceCounter(&_start_time);
}

double Timer::Stop()
{
	::QueryPerformanceCounter(&_end_time);

	return (double)(_end_time.QuadPart - _start_time.QuadPart) / (double)_freq.QuadPart;
}
