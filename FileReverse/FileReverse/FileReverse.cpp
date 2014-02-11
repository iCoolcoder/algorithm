// FileReverse.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FileReverse.h"
#include <Windows.h>
#include "CmnHdr.h"

BOOL _stdcall FileReverse(PCTSTR pszPathname, PBOOL pbIsTextUnicode)
{
	*pbIsTextUnicode = FALSE; // Assume text is Unicode

	// Open the file for reading and writing
	HANDLE hFile = CreateFile(pszPathname, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		chMB("File cannot be opened");
		return(FALSE);
	}

	// Get the size of the file (Assume the whole file can be mapped)
	DWORD dwFileSize = GetFileSize(hFile, NULL);

	// Create the file mapping object, the file mapping object is 1 character bigger than the file size so that a zero character
	// can be placed at the end of the file to terminate the string (file). Because I don't yet know if the file contains ANSI or
	// Unicode character, I assume the worst case, and add the size of a WCHAR instead of CHAR.
	HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, dwFileSize + sizeof(WCHAR), NULL);

	if (NULL == hFileMap)
	{
		chMB("File Map could not be opened");
		CloseHandle(hFile);
		CloseHandle(hFileMap);
		return(FALSE);
	}
	
	// Get the address where the first byte of the file is mapped into memory.
	PVOID pvFile = MapViewOfFile(hFileMap, FILE_MAP_WRITE, 0, 0, 0);

		if (NULL == pvFile)
	{
		chMB("Could not map view of file");
		CloseHandle(hFile);
		CloseHandle(hFileMap);
		return(FALSE);
	}

	// Does the buffer contain ANSI or Unicode?
	int iUnicodeTestFlags = -1; // Try all tests
	*pbIsTextUnicode = IsTextUnicode(pvFile, dwFileSize, &iUnicodeTestFlags);

	if (!*pbIsTextUnicode)
	{
		// For all the file manipulations below, we explicitly use ANSI functions because
		// we are processing an ANSI file.

		// Put a zero character at the very end of the file.
		PSTR pchANSI = (PSTR)pvFile;
		pchANSI[dwFileSize / sizeof(CHAR)] = 0;

		// Reverse the contents of the file
		_strrev(pchANSI);

		// Convert all "\n\r" combinations back to "\r\n" to preserve
		// the normal end-of-line sequence.
		pchANSI = strstr(pchANSI, "\n\r"); // Find first "\r\n".

		while (NULL != pchANSI)
		{
			// We have found an occurrence....
			*pchANSI++ = '\r'; // Change '\n' to '\r'
			*pchANSI++ = '\n'; // Change '\r' to '\n'
			pchANSI = strstr(pchANSI, "\n\r"); // Find the next occurrence
		}
	}
	else
	{
		// For all the file manipulations below, we explicitly use Unicode
		// functions because we are processing a Unicode file.

		// Put a zero character at the very end of the file.
		PWSTR pchUnicode = (PWSTR)pvFile;
		pchUnicode[dwFileSize / sizeof(WCHAR)] = 0;

		if ((iUnicodeTestFlags & IS_TEXT_UNICODE_SIGNATURE) != 0) 
		{
			// If the first character si the Unicode BOM (byte-order-mark),
			// 0xFFFF, keep this character at the very beginning of the file.
			pchUnicode++;
		}

		// Reverse the contents of the file.
		_wcsrev(pchUnicode);

		// Convert all "\n\r" combinations back to "\r\n" to preserve the normal end-of-line sequence.
		pchUnicode = wcsstr(pchUnicode, L"\n\r"); // Find first "\n\r"

		while (NULL != pchUnicode)
		{
			// We have found an occurrence....
			*pchUnicode++ = L'\r'; // Change '\n' to '\r'
			*pchUnicode++ = L'\n'; // Change '\r' to '\n'
			pchUnicode = wcsstr(pchUnicode, L"\n\r"); //Find the next occurence
		}
	}

	// Clean up everything before exiting
	UnmapViewOfFile(pvFile);
	CloseHandle(hFileMap);

	// Remove trailing zero character added earlier.
	SetFilePointer(hFile, dwFileSize, NULL, FILE_BEGIN);
	SetEndOfFile(hFile);
	CloseHandle(hFile);

	return(TRUE);
}

__int64 _stdcall Count0s(PCTSTR pszPathname)
{
	// Views must always start on a multiple of the allocation granularity
	SYSTEM_INFO sinf;
	GetSystemInfo(&sinf);

	// Open the data file.
	HANDLE hFile = CreateFile(pszPathname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	// Create file mapping object
	HANDLE hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);

	DWORD dwFileSizeHigh;
	__int64 qwFileSize = GetFileSize(hFile, &dwFileSizeHigh);
	qwFileSize += (((__int64)dwFileSizeHigh) << 32);

	// We no longer need access to the file object's handle
	CloseHandle(hFile);

	__int64 qwFileOffset = 0, qwNumOf0s = 0;

	while(qwFileSize > 0)
	{
		// Determine the number of bytes to be mapped in this view
		DWORD dwBytesInBlock = sinf.dwAllocationGranularity;
		if (qwFileSize < sinf.dwAllocationGranularity)
			dwBytesInBlock = (DWORD)qwFileSize;

		PBYTE pbFile = (PBYTE)MapViewOfFile(hFileMapping, FILE_MAP_READ, 
			(DWORD)(qwFileOffset >> 32),   // Starting byte
			(DWORD)(qwFileOffset & 0xFFFFFFFF),    //in file
			dwBytesInBlock);

		// Count the number of 0s in this block
		for (DWORD dwByte = 0; dwByte < dwBytesInBlock; dwByte++)
		{
			if (pbFile[dwByte] == 0)
				qwNumOf0s++;
		}

		// Unmap the view; we don't want multiple views in our address space
		UnmapViewOfFile(pbFile);

		// Skip to the next set of bytes in the file.
		qwFileOffset += dwBytesInBlock;
		qwFileSize -= dwBytesInBlock;
	}

	CloseHandle(hFileMapping);
	return(qwNumOf0s);
}
