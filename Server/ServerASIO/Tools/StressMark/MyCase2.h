
// ��ʾ�õĲ���������

#include "stdafx.h"
#include "StressCase.h"

class MyCase2 : public StressCase
{
public:
	DECLARE_DYNCREATE(MyCase2)

	bool OneStep(CString& strErrorMessage);
};