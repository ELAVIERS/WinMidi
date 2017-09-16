#include "FileDialogManager.h"

#include "Error.h"
#include <ShObjIdl.h>

bool FileDialogManager::OpenFile(std::string& str)
{
	IFileOpenDialog* open_dialog;
	COMDLG_FILTERSPEC filters[] = { { L"MIDI Files", L"*.MID;*.MIDI" },{ L"All Files", L"*.*" } };

	HRESULT result = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&open_dialog));

	if (SUCCEEDED(result)) {
		open_dialog->SetFileTypes(2, filters);
		open_dialog->Show(NULL);

		LPWSTR file_name;
		IShellItem* item;

		result = open_dialog->GetResult(&item);
		if (!SUCCEEDED(result))
			return false;

		item->GetDisplayName(SIGDN_FILESYSPATH, &file_name);

		str.clear();
		for (int i = 0; file_name[i] != '\0'; ++i)
			str.push_back((char)file_name[i]);

		return true;
	}

	Error::ErrorMessage("Could not open file dialog");
	return false;
}