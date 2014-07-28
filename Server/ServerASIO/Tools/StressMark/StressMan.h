////////////////////////////////////////////////////////////////////////////
//
// StressMan.h - StressMark 管理所有测试用例（线程）的类
//
// created by Wang Yong Gang, 2001-08-07 ( version 1.0 )
// modified by Wang Yong Gang, 2002-02-16 ( version 1.1 )
//
////////////////////////////////////////////////////////////////////////////

#if !defined(_WIX_STRESS_MAN_H_001)
#define _WIX_STRESS_MAN_H_001

class StressMan
{
public:
	StressMan(CListCtrl& list);
	~StressMan();
public:
	// 初始化用例列表
	bool InitializeCaseList();

protected:
	// 所有的线程指针(CWinThread*)
	CPtrArray m_arrThreads;

	// 监视线程指针(CWinThread*)
	CWinThread* m_pMonitorThread;

	// 配置数组 g_arrTestSuite 的大小
	int m_nTestSuiteArraySize;

	// 所有的测试用例对象指针(StressCase*)
	CObArray m_arrCaseObjects;	

	// 传递给用例线程的初始化信息结构
	struct ThreadInfo
	{
		StressMan* pStressMan;
		int nIndex;		// 线程序号（在m_arrCaseObjects中的）
		int nTimes;		// 该线程内重复测试（调用OnStep()）的次数
		int nInterval;	// 每两次重复中间间隔的时间（微秒数）
	};

	// 所有用例线程的初始化信息
	ThreadInfo* m_arrThreadInfo;

	// 监视线程的初始化信息
	ThreadInfo m_MonitorThreadInfo;

	// 用例列表控件
	CListCtrl& m_list;	

	// 同步所有用例线程的事件
	CEvent m_event;

	// 表示有多少个线程存活的记数器
	LONG m_lives;

public:
	// 启动
	bool StartSuite();

	// 停止
	bool StopSuite();

	// 无法终止时杀线程
	bool KillSuite();

	// 报表
	bool ReportSuite(LPCTSTR strPath);

protected:
	// 设置线程状态并更新列表框显示
	//		nIndex: 序号，-1表示设置所有线程状态
	//		nState: 状态类型，见StressCase的枚举
	//		strMessage: 描述信息
	void SetThreadState(int nIndex, int nState, LPCTSTR strMessage = NULL);

	// 设置线程已执行次数并更新列表框显示
	//		nIndex: 序号，-1表示设置所有线程
	//		nTimes: 执行次数
	void StressMan::SetThreadFinishedTimes(int nIndex, int nTimes);

	// 设置线程执行总时间并更新列表框显示
	//		nIndex: 序号，-1表示设置所有线程状态
	//		dTime: 执行总时间
	void StressMan::SetThreadTime(int nIndex, double dTime);

	// 线程的执行体
	static UINT ThreadBody( LPVOID pParam );

	// 监视线程的执行体
	static UINT MonitorThreadBody( LPVOID pParam );

	// 根据测试逻辑，判断是否能设置为新的状态
	inline bool CanSetState(int oldState, int newState);
};

#endif