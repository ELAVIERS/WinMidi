#include "FileIO.h"

#include <Windows.h>

void FileIO::BufferToFile(unsigned char *buffer, unsigned int buffer_len, const char *filepath) {
	HANDLE file = CreateFileA(filepath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!file) return;

	WriteFile(file, (LPCVOID)buffer, (DWORD)buffer_len, NULL, NULL);

	CloseHandle(file);
}
