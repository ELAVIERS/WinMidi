#pragma once
#include <Windows.h>

class Timer
{
public:
	Timer();
	~Timer();

	void Start();
	//void Pause();
	double Stop();

protected:
	LARGE_INTEGER _start_time;
	LARGE_INTEGER _end_time;

	//LARGE_INTEGER _time;
	LARGE_INTEGER _freq;
};

