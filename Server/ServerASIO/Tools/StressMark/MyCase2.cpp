
// 演示用的测试用例类

#include "stdafx.h"
#include "StressCase.h"
#include "MyCase2.h"

IMPLEMENT_DYNCREATE(MyCase2, StressCase)

bool MyCase2::OneStep(CString& strErrorMessage)
{
	// do something...
	Sleep(500);		
	return true;
}
