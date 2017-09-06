#pragma once
#include <Windows.h>

class Timer
{
public:
	Timer();
	~Timer();

	void start();
	float stop();

protected:
	LARGE_INTEGER _start_time;
	LARGE_INTEGER _freq;
};

