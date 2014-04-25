// stdio.c
#include "minicrt.h"

int mini_crt_io_init()
{
	return 1;
}

#ifdef WIN32
#include <Windows.h>
FILE* mini_fopen(const char* filename, const char* mode)
{
	HANDLE hFile = 0;
	int access = 0;
	int creation = 0;

	if(mini_strcmp(mode, "w") == 0)
	{
		access |= GENERIC_WRITE;
		creation |= CREATE_ALWAYS;
	}

	if(mini_strcmp(mode, "w+") == 0)
	{
		access |= GENERIC_WRITE | GENERIC_READ;
		creation |= CREATE_ALWAYS;
	}

	if(mini_strcmp(mode, "r") == 0)
	{
		access |= GENERIC_READ;
		creation |= OPEN_EXISTING;
	}

	if(strcmp(mode, "r+") == 0)
	{
		access |= GENERIC_WRITE | GENERIC_READ;
		creation |= TRUNCATE_EXISTING;
	}

	hFile = CreateFileA(filename, access, 0, 0, creation, 0, 0);
	if(hFile == INVALID_HANDLE_VALUE)
		return 0;

	return (FILE*)hFile;
}


#endif