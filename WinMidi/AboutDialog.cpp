#include "AboutDialog.h"
#include "resource.h"

void AboutDialog::Open(HINSTANCE instance, HWND owner)
{
	::DialogBox(instance, MAKEINTRESOURCE(IDD_ABOUT), owner, Procedure);
}

INT_PTR CALLBACK AboutDialog::Procedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		return 1;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			::EndDialog(hWnd, LOWORD(wParam));
			return 1;
		}
		break;
	}

	return 0;
}
