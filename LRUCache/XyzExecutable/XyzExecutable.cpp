// XyzExecutable.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "XyzLibrary.h"

#if _MSC_VER > 1200     // VC++7.0 and higher
#include "AutoClosePtr.h"
#elif _MSC_VER == 1200  // VC++6.0
#include "AutoClosePtrVC6.h"
#else                   // really old version
#error  Newer MS VC++ compiler required.
#endif // _MSC_VER

int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
	LRUCache<char*> cache(5 * 1024);
	char* str = "xyz";
	char* temp = new char[100];
	for (int i = 0; i < 6 * 1024 * 1024; i++)
	{
		_itoa_s(i, temp, 100, 10);
		string key(temp);
		cache.Add(key, str, 4);
		printf("add one\n");
	}
	return 0;
}

