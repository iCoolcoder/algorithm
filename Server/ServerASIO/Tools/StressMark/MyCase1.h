
// 演示用的测试用例类

#include "stdafx.h"
#include "StressCase.h"

#define PORT 8100
#define IP_ADDRESS "10.172.56.150"

class MyCase1 : public StressCase
{
public:
	DECLARE_DYNCREATE(MyCase1)

	bool OnInitialize(CString& strErrorMessage);

	bool OnUninitialize(CString& strErrorMessage);

	bool OneStep(CString& strErrorMessage);
};