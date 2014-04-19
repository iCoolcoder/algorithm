// XyzExecutable.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "XyzLibrary.h"
#include "Queue.h"
#include "Stack.h"

#if _MSC_VER > 1200     // VC++7.0 and higher
#include "AutoClosePtr.h"
#elif _MSC_VER == 1200  // VC++6.0
#include "AutoClosePtrVC6.h"
#else                   // really old version
#error  Newer MS VC++ compiler required.
#endif // _MSC_VER

int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
	Stack q;
	q.Push(3);
	int data;
	while(!q.IsEmpty())
	{
		q.Pop(data);
		printf("%d\n", data);
	}

	return 0;
}

