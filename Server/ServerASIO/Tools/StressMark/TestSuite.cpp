///////////////////////////////////////////////////////////////////////////
//
// TestSuite.cpp - 测试包定义
//
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StressCase.h"

//
// 在此处包含所有具体的测试用例类的头文件，例如
// #include "MyCase1.h"
// #include "MyCase2.h"
// ...
//
#include "MyCase1.h"
#include "MyCase2.h"

//
// 此处定义所有具体的测试用例
//		每个具体测试用例的定义语法为
//
//		BEGIN_TEST_SUITE
//			DECLARE_TEST_CASE(ClassName, Name, Clients, Times, Interval)
//			...
//		END_TEST_SUITE
//
//			其中，ClassName 是测试用例的类名，
//			Name 是显示用的测试用例名称，
//			Clients 是指定为该测试用例起多少个客户（线程）
//			Times 是每个客户（线程）中重复测试多少次
//			Interval 是线程中每两次测试间的时间间隔（微秒数，
//				Interval 为 0 表示不间断地连续测试）
//
//
BEGIN_TEST_SUITE
	DECLARE_TEST_CASE(MyCase1, "Testcase1", 5000, 5, 10)
	DECLARE_TEST_CASE(MyCase2, "Testcase2", 8, 10, 0)
END_TEST_SUITE

