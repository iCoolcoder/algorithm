///////////////////////////////////////////////////////////////////////////
//
// TestSuite.cpp - ���԰�����
//
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StressCase.h"

//
// �ڴ˴��������о���Ĳ����������ͷ�ļ�������
// #include "MyCase1.h"
// #include "MyCase2.h"
// ...
//
#include "MyCase1.h"
#include "MyCase2.h"

//
// �˴��������о���Ĳ�������
//		ÿ��������������Ķ����﷨Ϊ
//
//		BEGIN_TEST_SUITE
//			DECLARE_TEST_CASE(ClassName, Name, Clients, Times, Interval)
//			...
//		END_TEST_SUITE
//
//			���У�ClassName �ǲ���������������
//			Name ����ʾ�õĲ����������ƣ�
//			Clients ��ָ��Ϊ�ò�����������ٸ��ͻ����̣߳�
//			Times ��ÿ���ͻ����̣߳����ظ����Զ��ٴ�
//			Interval ���߳���ÿ���β��Լ��ʱ������΢������
//				Interval Ϊ 0 ��ʾ����ϵ��������ԣ�
//
//
BEGIN_TEST_SUITE
	DECLARE_TEST_CASE(MyCase1, "Testcase1", 5000, 5, 10)
	DECLARE_TEST_CASE(MyCase2, "Testcase2", 8, 10, 0)
END_TEST_SUITE

