////////////////////////////////////////////////////////////////////////////
//
// StressCase.h - 所有测试用例类的基类
//
// created by Wang Yong Gang, 2001-08-07 ( version 1.0 )
// modified by Wang Yong Gang, 2002-02-16 ( version 1.1 )
//
////////////////////////////////////////////////////////////////////////////

#if !defined(_WIX_STRESS_CASE_H_001_)
#define _WIX_STRESS_CASE_H_001_

// TestSuite 中定义测试包时要用到的宏
#define BEGIN_TEST_SUITE	\
	struct StressCaseDefination g_arrTestSuite[] = {
#define DECLARE_TEST_CASE( ClassName, Name, Clients, Times, Interval )	\
	{ RUNTIME_CLASS(ClassName), Name, Clients, Times, Interval },
#define END_TEST_SUITE		\
	{ NULL, NULL, 0, 0, 0 } };

//
// 测试用例类的基类，所有实际的测试用例类都是此类的子类
//		此类或其子类的每一个运行时对象对应于一个测试客户（线程）
//		同一个测试用例可以拥有多个测试客户（线程），即多个对象
//
class StressCase : public CObject
{	
public:
	DECLARE_DYNCREATE(StressCase)	

	// 构造函数
	StressCase();

	// 析构函数
	~StressCase() {};

public:
	// 子类可以重载此方法以完成测试前的初始化工作
	// 如果失败，错误信息可以通过strErrorMessage返回
	// 返回值表示初始化成功与否
	virtual bool OnInitialize(CString& strErrorMessage) { return true; };

	// 子类可以重载此方法以完成测试后的清理工作
	// 如果失败，错误信息可以在strErrorMessage中返回
	// 返回值表示清理成功与否
	virtual bool OnUninitialize(CString& strErrorMessage) { return true; };

	// 子类必须重载此方法，并在此方法中完成每次测试的实际操作
	// 如果失败，错误信息可以在strErrorMessage中返回
	// 返回值表示测试成功与否	
	virtual bool OneStep(CString& strErrorMessage) { return true; };

public:	
	// 客户（线程）所属的测试用例名称
	CString m_strName;

	// 客户（线程）的总序号（所有线程的总序号，从1开始）
	int m_nID;

	// 客户（线程）的线程号（同一测试用例中的线程序号，从1开始）
	int m_nClientID;

	// 客户（线程）的当前执行状态代码（见下面的枚举）
	int m_nState;

	// 客户（线程）的当前执行状态描述（发生错误时包含错误信息）
	CString m_strState;

	// 客户（线程）已执行的次数
	int m_nFinishedTimes;

	// 客户（线程）的总执行时间
	//（以秒为单位，不包括每两次测试之间设定的间隔时间Interval）
	double m_dTime;

public:
	// 客户（线程）状态的枚举
	static enum
	{
		ready,		// 就绪
		starting,	// 启动过程中
		running,	// 运行
		stopping,	// 停止过程中
		error		// 错误终止
	};		

	// 写应用程序日志（仅在NT平台有效)
	void LogEvent(LPCTSTR strMessage);

	// 设置客户（线程）的当前状态
	//		nState: 状态类型，见上面的枚举
	//		strMessage: 描述信息
	void SetClientState(int nState, LPCTSTR strMessage = NULL);	
};

//
// 测试用例定义类，用于在 TestSuite.cpp 中定义和配置不同的测试用例
//
struct StressCaseDefination
{
	// 测试用例的类名，使用时以 RUNTIME_CLASS(ClassName) 方式给出
	CRuntimeClass* m_pCaseClass;

	// 测试用例的显示名称
	LPCTSTR m_strName;

	// 测试用例需要模拟的客户（线程）数目，最小值1，最大值100
	//		即对每个测试用例类启动多少个对象实例（线程）
	int m_nClients;

	// 每客户（线程）中测试的重复次数，最小值1，最大值10000
	//		即每线程中重复调用多少次测试用例类的 OneStep() 方法
	int m_nTimes;

	// 每客户（线程）中执行 OneStep() 方法之间间隔的时间（微秒数）
	int m_nInterval;
};

#endif