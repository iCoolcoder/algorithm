// XyzExecutable.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "XyzLibrary.h"
#include "Queue.h"
#include "Stack.h"
#include "BiTree.h"
#include "Smart_Ptr.h"

#if _MSC_VER > 1200     // VC++7.0 and higher
#include "AutoClosePtr.h"
#elif _MSC_VER == 1200  // VC++6.0
#include "AutoClosePtrVC6.h"
#else                   // really old version
#error  Newer MS VC++ compiler required.
#endif // _MSC_VER

int compare(int a, int b)
{
	int ret = 0;
	if(a > b)
	{
		ret = 1;
	}
	else if (a < b)
		ret = -1;
	else
		ret = 0;

	return ret;
}

int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
	Smart_Ptr<int> x(new int(12));
	printf("%d\n", *x.operator->());

	BiTree bTree;

	bTree.Insert(1, compare);
	bTree.Insert(0, compare);
	bTree.Insert(2, compare);
	bTree.Insert(3, compare);
	bTree.Insert(4, compare);
	
	printf("%d\n", bTree.IsBalanced_Recursive());
	return 0;
}

