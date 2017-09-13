#include "WindowClass.h"

#include "Error.h"

WindowClass::WindowClass()
{
}


WindowClass::~WindowClass()
{
	::UnregisterClass(_name, _instance);
}

void WindowClass::Initialise(LPCTSTR class_name, HINSTANCE instance, LPCTSTR menu, WNDPROC window_procedure)
{
	WNDCLASSEX window_class
	{
		sizeof(WNDCLASSEX),
		0,									//Style
		window_procedure,					//WNDPROC
		0,									//Extra class bytes
		0,									//Extra window bytes
		instance,							//HINSTANCE containing WNDPROC
		::LoadIcon(NULL, IDI_APPLICATION),	//Icon handle
		::LoadCursor(NULL, IDC_ARROW),		//Cursor handle
		(HBRUSH)(COLOR_WINDOW + 1),			//Background brush
		menu,								//Menu name
		class_name,							//Class name
		::LoadIcon(NULL, IDI_APPLICATION)	//Small Icon
	};

	if (!::RegisterClassEx(&window_class))
		Error::ErrorMessageFatal("Unable to register Window Class");
}
