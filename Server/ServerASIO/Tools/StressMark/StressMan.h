////////////////////////////////////////////////////////////////////////////
//
// StressMan.h - StressMark �������в����������̣߳�����
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
	// ��ʼ�������б�
	bool InitializeCaseList();

protected:
	// ���е��߳�ָ��(CWinThread*)
	CPtrArray m_arrThreads;

	// �����߳�ָ��(CWinThread*)
	CWinThread* m_pMonitorThread;

	// �������� g_arrTestSuite �Ĵ�С
	int m_nTestSuiteArraySize;

	// ���еĲ�����������ָ��(StressCase*)
	CObArray m_arrCaseObjects;	

	// ���ݸ������̵߳ĳ�ʼ����Ϣ�ṹ
	struct ThreadInfo
	{
		StressMan* pStressMan;
		int nIndex;		// �߳���ţ���m_arrCaseObjects�еģ�
		int nTimes;		// ���߳����ظ����ԣ�����OnStep()���Ĵ���
		int nInterval;	// ÿ�����ظ��м�����ʱ�䣨΢������
	};

	// ���������̵߳ĳ�ʼ����Ϣ
	ThreadInfo* m_arrThreadInfo;

	// �����̵߳ĳ�ʼ����Ϣ
	ThreadInfo m_MonitorThreadInfo;

	// �����б�ؼ�
	CListCtrl& m_list;	

	// ͬ�����������̵߳��¼�
	CEvent m_event;

	// ��ʾ�ж��ٸ��̴߳��ļ�����
	LONG m_lives;

public:
	// ����
	bool StartSuite();

	// ֹͣ
	bool StopSuite();

	// �޷���ֹʱɱ�߳�
	bool KillSuite();

	// ����
	bool ReportSuite(LPCTSTR strPath);

protected:
	// �����߳�״̬�������б����ʾ
	//		nIndex: ��ţ�-1��ʾ���������߳�״̬
	//		nState: ״̬���ͣ���StressCase��ö��
	//		strMessage: ������Ϣ
	void SetThreadState(int nIndex, int nState, LPCTSTR strMessage = NULL);

	// �����߳���ִ�д����������б����ʾ
	//		nIndex: ��ţ�-1��ʾ���������߳�
	//		nTimes: ִ�д���
	void StressMan::SetThreadFinishedTimes(int nIndex, int nTimes);

	// �����߳�ִ����ʱ�䲢�����б����ʾ
	//		nIndex: ��ţ�-1��ʾ���������߳�״̬
	//		dTime: ִ����ʱ��
	void StressMan::SetThreadTime(int nIndex, double dTime);

	// �̵߳�ִ����
	static UINT ThreadBody( LPVOID pParam );

	// �����̵߳�ִ����
	static UINT MonitorThreadBody( LPVOID pParam );

	// ���ݲ����߼����ж��Ƿ�������Ϊ�µ�״̬
	inline bool CanSetState(int oldState, int newState);
};

#endif