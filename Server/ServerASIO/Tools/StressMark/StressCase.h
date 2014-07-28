////////////////////////////////////////////////////////////////////////////
//
// StressCase.h - ���в���������Ļ���
//
// created by Wang Yong Gang, 2001-08-07 ( version 1.0 )
// modified by Wang Yong Gang, 2002-02-16 ( version 1.1 )
//
////////////////////////////////////////////////////////////////////////////

#if !defined(_WIX_STRESS_CASE_H_001_)
#define _WIX_STRESS_CASE_H_001_

// TestSuite �ж�����԰�ʱҪ�õ��ĺ�
#define BEGIN_TEST_SUITE	\
	struct StressCaseDefination g_arrTestSuite[] = {
#define DECLARE_TEST_CASE( ClassName, Name, Clients, Times, Interval )	\
	{ RUNTIME_CLASS(ClassName), Name, Clients, Times, Interval },
#define END_TEST_SUITE		\
	{ NULL, NULL, 0, 0, 0 } };

//
// ����������Ļ��࣬����ʵ�ʵĲ��������඼�Ǵ��������
//		������������ÿһ������ʱ�����Ӧ��һ�����Կͻ����̣߳�
//		ͬһ��������������ӵ�ж�����Կͻ����̣߳������������
//
class StressCase : public CObject
{	
public:
	DECLARE_DYNCREATE(StressCase)	

	// ���캯��
	StressCase();

	// ��������
	~StressCase() {};

public:
	// ����������ش˷�������ɲ���ǰ�ĳ�ʼ������
	// ���ʧ�ܣ�������Ϣ����ͨ��strErrorMessage����
	// ����ֵ��ʾ��ʼ���ɹ����
	virtual bool OnInitialize(CString& strErrorMessage) { return true; };

	// ����������ش˷�������ɲ��Ժ��������
	// ���ʧ�ܣ�������Ϣ������strErrorMessage�з���
	// ����ֵ��ʾ����ɹ����
	virtual bool OnUninitialize(CString& strErrorMessage) { return true; };

	// ����������ش˷��������ڴ˷��������ÿ�β��Ե�ʵ�ʲ���
	// ���ʧ�ܣ�������Ϣ������strErrorMessage�з���
	// ����ֵ��ʾ���Գɹ����	
	virtual bool OneStep(CString& strErrorMessage) { return true; };

public:	
	// �ͻ����̣߳������Ĳ�����������
	CString m_strName;

	// �ͻ����̣߳�������ţ������̵߳�����ţ���1��ʼ��
	int m_nID;

	// �ͻ����̣߳����̺߳ţ�ͬһ���������е��߳���ţ���1��ʼ��
	int m_nClientID;

	// �ͻ����̣߳��ĵ�ǰִ��״̬���루�������ö�٣�
	int m_nState;

	// �ͻ����̣߳��ĵ�ǰִ��״̬��������������ʱ����������Ϣ��
	CString m_strState;

	// �ͻ����̣߳���ִ�еĴ���
	int m_nFinishedTimes;

	// �ͻ����̣߳�����ִ��ʱ��
	//������Ϊ��λ��������ÿ���β���֮���趨�ļ��ʱ��Interval��
	double m_dTime;

public:
	// �ͻ����̣߳�״̬��ö��
	static enum
	{
		ready,		// ����
		starting,	// ����������
		running,	// ����
		stopping,	// ֹͣ������
		error		// ������ֹ
	};		

	// дӦ�ó�����־������NTƽ̨��Ч)
	void LogEvent(LPCTSTR strMessage);

	// ���ÿͻ����̣߳��ĵ�ǰ״̬
	//		nState: ״̬���ͣ��������ö��
	//		strMessage: ������Ϣ
	void SetClientState(int nState, LPCTSTR strMessage = NULL);	
};

//
// �������������࣬������ TestSuite.cpp �ж�������ò�ͬ�Ĳ�������
//
struct StressCaseDefination
{
	// ����������������ʹ��ʱ�� RUNTIME_CLASS(ClassName) ��ʽ����
	CRuntimeClass* m_pCaseClass;

	// ������������ʾ����
	LPCTSTR m_strName;

	// ����������Ҫģ��Ŀͻ����̣߳���Ŀ����Сֵ1�����ֵ100
	//		����ÿ�������������������ٸ�����ʵ�����̣߳�
	int m_nClients;

	// ÿ�ͻ����̣߳��в��Ե��ظ���������Сֵ1�����ֵ10000
	//		��ÿ�߳����ظ����ö��ٴβ���������� OneStep() ����
	int m_nTimes;

	// ÿ�ͻ����̣߳���ִ�� OneStep() ����֮������ʱ�䣨΢������
	int m_nInterval;
};

#endif