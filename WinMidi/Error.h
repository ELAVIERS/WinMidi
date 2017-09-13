#pragma once
#include <Windows.h>

namespace Error
{
	inline void ErrorMessage(const char* Error)
	{
		::MessageBoxA(NULL, Error, "Error", MB_OK);
	}

	inline void ErrorMessageFatal(const char* Error)
	{
		::MessageBeep(MB_ICONERROR);
		::MessageBoxA(NULL, Error, "Fatal Error!", MB_OK);
		exit(0);
	}

}
