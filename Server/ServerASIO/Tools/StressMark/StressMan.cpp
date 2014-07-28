////////////////////////////////////////////////////////////////////////////
//
// StressMan.cpp - StressMark 管理所有测试用例（线程）的类
//
// created by Wang Yong Gang, 2001-08-07 ( version 1.0 )
// modified by Wang Yong Gang, 2002-02-16 ( version 1.1 )
//
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "StressCase.h"
#include "StressMan.h"

extern struct StressCaseDefination g_arrTestSuite[];

StressMan::StressMan(CListCtrl& list)
	: m_list(list), m_event(FALSE, TRUE)
{
	m_arrThreadInfo = NULL;
	m_nTestSuiteArraySize = 0;
}

StressMan::~StressMan()
{
	for(int i = 0; i < m_arrCaseObjects.GetSize(); i++)
	{
		StressCase* p = (StressCase*)m_arrCaseObjects.GetAt(i);
		delete p;
	}
	m_arrCaseObjects.RemoveAll();
	m_arrThreads.RemoveAll();
	if (m_arrThreadInfo)
		delete[] m_arrThreadInfo;
}

bool StressMan::InitializeCaseList()
{	
	m_arrCaseObjects.RemoveAll();

	// 统计测试用例有多少种，并根据设置的线程数为
	// 每种测试用例创建若干对象实例
	int nIndex = 0; StressCase* p;
	m_nTestSuiteArraySize = 0;
	while(g_arrTestSuite[m_nTestSuiteArraySize].m_pCaseClass != NULL)
	{
		g_arrTestSuite[m_nTestSuiteArraySize].m_nClients = 
			min(100, max(1, g_arrTestSuite[m_nTestSuiteArraySize].m_nClients));
		g_arrTestSuite[m_nTestSuiteArraySize].m_nTimes = 
			min(10000, max(1, g_arrTestSuite[m_nTestSuiteArraySize].m_nTimes));
		for(int i = 0; i < g_arrTestSuite[m_nTestSuiteArraySize].m_nClients; i++)
		{
			p = (StressCase*)(g_arrTestSuite[m_nTestSuiteArraySize]
					.m_pCaseClass->CreateObject());
			p->m_strName = g_arrTestSuite[m_nTestSuiteArraySize].m_strName;
			p->m_nID = nIndex + 1;
			p->m_nClientID = i + 1;
			m_arrCaseObjects.Add(p);
			nIndex++;
		}
		m_nTestSuiteArraySize++;
	}
	if (!m_nTestSuiteArraySize)
	{
		AfxMessageBox(_T("未定义任何测试用例"));
		return false;
	}
	m_arrThreadInfo = new ThreadInfo[m_arrCaseObjects.GetSize()];
	if (m_arrThreadInfo == NULL)
	{
		AfxMessageBox(_T("内存不足"));
		return false;
	}

	// 显示所有线程信息
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	
	m_list.InsertColumn(0, _T("序号"), LVCFMT_LEFT, 40);
	m_list.InsertColumn(1, _T("测试用例名"), LVCFMT_LEFT, 130);
	m_list.InsertColumn(2, _T("线程"), LVCFMT_LEFT, 40);
	m_list.InsertColumn(3, _T("计划执行次数"), LVCFMT_RIGHT, 90);
	m_list.InsertColumn(4, _T("已执行次数"), LVCFMT_RIGHT, 80);
	m_list.InsertColumn(5, _T("平均每次耗时(秒)"), LVCFMT_RIGHT, 120);
	m_list.InsertColumn(6, _T("当前状态"), LVCFMT_LEFT, 300);

	nIndex = 0; TCHAR temp[20];
	for(int i = 0; i < m_nTestSuiteArraySize; i++)
	{
		for(int j = 0; j < g_arrTestSuite[i].m_nClients; j++)
		{
			StressCase* p = (StressCase*)m_arrCaseObjects.GetAt(nIndex);

			_stprintf(temp, _T("%03d"), nIndex + 1);
			m_list.InsertItem(nIndex, temp);
			m_list.SetItem(nIndex, 1, LVIF_TEXT, 
				g_arrTestSuite[i].m_strName, 0, 0, 0, 0);
			_stprintf(temp, _T("%03d"), j + 1);
			m_list.SetItem(nIndex, 2, LVIF_TEXT, temp, 0, 0, 0, 0);
			_stprintf(temp, _T("%d"), g_arrTestSuite[i].m_nTimes);
			m_list.SetItem(nIndex, 3, LVIF_TEXT, temp, 0, 0, 0, 0);
			_stprintf(temp, _T("%d"), p->m_nFinishedTimes);
			m_list.SetItem(nIndex, 4, LVIF_TEXT, temp, 0, 0, 0, 0);
			_stprintf(temp, _T("%.3f"), 
				(p->m_nFinishedTimes != 0) ? p->m_dTime / 
				(double)p->m_nFinishedTimes : 0.0);
			m_list.SetItem(nIndex, 5, LVIF_TEXT, temp, 0, 0, 0, 0);
			m_list.SetItem(nIndex, 6, LVIF_TEXT, p->m_strState, 0, 0, 0, 0);
			nIndex++;
		}
	}	
	return true;
}

bool StressMan::CanSetState(int oldState, int newState)
{
	// 当原状态是 ready 或 error 时，
	// 不能设置新状态为 ready 或 stopping
	if ((oldState == StressCase::error || oldState == StressCase::ready)
			&& ( newState == StressCase::ready || newState == StressCase::stopping))
		return false;
	else 
		return true;
}

void StressMan::SetThreadState(int nIndex, int nState, LPCTSTR strMessage)
{
	int nCount = 0;
	if (nIndex < 0)
	{
		for(int i = 0; i < m_nTestSuiteArraySize; i++)
		{
			for(int j = 0; j < g_arrTestSuite[i].m_nClients; j++)
			{
				StressCase* p = (StressCase*)m_arrCaseObjects.GetAt(nCount);				
				if (CanSetState(p->m_nState, nState))
				{
					p->SetClientState(nState, strMessage);
					m_list.SetItem(nCount, 6, LVIF_TEXT, p->m_strState, 0, 0, 0, 0);
				}
				nCount++;
			}
		}
	}
	else
	{
		StressCase* p = (StressCase*)m_arrCaseObjects.GetAt(nIndex);
		if (CanSetState(p->m_nState, nState))
		{
			p->SetClientState(nState, strMessage);
			m_list.SetItem(nIndex, 6, LVIF_TEXT, p->m_strState, 0, 0, 0, 0);
		}
	}

}

void StressMan::SetThreadFinishedTimes(int nIndex, int nTimes)
{
	int nCount = 0; TCHAR temp[20];
	if (nIndex < 0)
	{
		for(int i = 0; i < m_nTestSuiteArraySize; i++)
		{
			for(int j = 0; j < g_arrTestSuite[i].m_nClients; j++)
			{
				StressCase* p = (StressCase*)m_arrCaseObjects.GetAt(nCount);
				p->m_nFinishedTimes = nTimes;
				_stprintf(temp, _T("%d"), p->m_nFinishedTimes);
				m_list.SetItem(nCount, 4, LVIF_TEXT, temp, 0, 0, 0, 0);
				nCount++;
			}
		}
	}
	else
	{
		StressCase* p = (StressCase*)m_arrCaseObjects.GetAt(nIndex);
		p->m_nFinishedTimes = nTimes;
		_stprintf(temp, _T("%d"), p->m_nFinishedTimes);
		m_list.SetItem(nIndex, 4, LVIF_TEXT, temp, 0, 0, 0, 0);
	}
}

void StressMan::SetThreadTime(int nIndex, double dTime)
{
	int nCount = 0; TCHAR temp[20];
	if (nIndex < 0)
	{
		for(int i = 0; i < m_nTestSuiteArraySize; i++)
		{
			for(int j = 0; j < g_arrTestSuite[i].m_nClients; j++)
			{
				StressCase* p = (StressCase*)m_arrCaseObjects.GetAt(nCount);
				p->m_dTime = dTime;
				_stprintf(temp, _T("%.3f"), 
					(p->m_nFinishedTimes != 0) ? p->m_dTime / 
					(double)p->m_nFinishedTimes : 0.0);
				m_list.SetItem(nCount, 5, LVIF_TEXT, temp, 0, 0, 0, 0);
				nCount++;
			}
		}
	}
	else
	{
		StressCase* p = (StressCase*)m_arrCaseObjects.GetAt(nIndex);
		p->m_dTime = dTime;
		_stprintf(temp, _T("%.3f"), 
			(p->m_nFinishedTimes != 0) ? p->m_dTime / 
			(double)p->m_nFinishedTimes : 0.0);
		m_list.SetItem(nIndex, 5, LVIF_TEXT, temp, 0, 0, 0, 0);
	}
}

bool StressMan::StartSuite()
{
	SetThreadState(-1, StressCase::starting);	
	SetThreadFinishedTimes(-1, 0);
	SetThreadTime(-1, 0.0);
	AfxGetMainWnd()->InvalidateRect(NULL);
	AfxGetMainWnd()->UpdateWindow();
	
	CWinThread* pThread;

	// 设置同步事件
	m_event.ResetEvent();

	// 线程活动数
	m_lives = 0;

	// 创建所有测试用例线程
	m_arrThreads.RemoveAll();

	for(int i = 0; i < m_nTestSuiteArraySize; i++)
	{
		for(int j = 0; j < g_arrTestSuite[i].m_nClients; j++)
		{
			m_arrThreadInfo[m_lives].pStressMan = this;
			m_arrThreadInfo[m_lives].nIndex = m_lives;
			m_arrThreadInfo[m_lives].nTimes = g_arrTestSuite[i].m_nTimes;
			m_arrThreadInfo[m_lives].nInterval = g_arrTestSuite[i].m_nInterval;
			pThread = AfxBeginThread(StressMan::ThreadBody, &(m_arrThreadInfo[m_lives]));
			m_arrThreads.Add(pThread);			
			m_lives++;
		}
	}	

	// 创建监视线程
	m_MonitorThreadInfo.pStressMan = this;
	m_MonitorThreadInfo.nIndex = 0;
	m_MonitorThreadInfo.nTimes = 0;
	m_pMonitorThread = AfxBeginThread(StressMan::MonitorThreadBody, &m_MonitorThreadInfo);	

	// 设置同步事件，同时启动所有用例线程
	m_event.SetEvent();

	return true;
}

bool StressMan::StopSuite()
{
	SetThreadState(-1, StressCase::stopping);
	return true;
}

bool StressMan::KillSuite()
{
	if (WaitForSingleObject(m_pMonitorThread->m_hThread, 0) != WAIT_OBJECT_0)
		TerminateThread(m_pMonitorThread->m_hThread, 0);

	int nCount = m_arrThreads.GetSize();	
	CWinThread* p;
	for(int i = 0; i < nCount; i++)
	{
		p = (CWinThread*)m_arrThreads.GetAt(i);
		if (WaitForSingleObject(p->m_hThread, 0) != WAIT_OBJECT_0)
			TerminateThread(p->m_hThread, 0);
	}
	SetThreadState(-1, StressCase::ready);
	return true;
}

UINT StressMan::ThreadBody( LPVOID pParam )
{
	ThreadInfo* pti = (ThreadInfo*)pParam;
	StressCase* pCase = (StressCase*)pti->pStressMan->m_arrCaseObjects.GetAt(pti->nIndex);
	CString strErrorMessage = _T("");
	LARGE_INTEGER freq, c1, c2;
	QueryPerformanceFrequency(&freq);
	double dTime; int i;
	
	// 初始化
	if (!pCase->OnInitialize(strErrorMessage))
	{
		pti->pStressMan->SetThreadState(pti->nIndex, StressCase::error, strErrorMessage);
		goto exit_pos;
	}	

	// 设置正运行标记
	pti->pStressMan->SetThreadState(pti->nIndex, StressCase::running);
	
	// 等待同步事件
	WaitForSingleObject(pti->pStressMan->m_event.m_hObject, 60 * 1000l);

	// 测试
	for(i = 0; i < pti->nTimes; i++)
	{
		if (pCase->m_nState == StressCase::stopping)
			break;

		QueryPerformanceCounter(&c1);
		if (!pCase->OneStep(strErrorMessage))
		{
			pti->pStressMan->SetThreadState(pti->nIndex, StressCase::error, strErrorMessage);
			break;
		}
		QueryPerformanceCounter(&c2);		

		// 刷新已执行次数和执行总时间
		pti->pStressMan->SetThreadFinishedTimes(pti->nIndex, i+1);
		dTime = pCase->m_dTime + 
			((double)c2.QuadPart - (double)c1.QuadPart) / (double)freq.QuadPart;
		pti->pStressMan->SetThreadTime(pti->nIndex, dTime);

		// 根据设置，休息一个时间间隔
		if (pti->nInterval)	Sleep(pti->nInterval);
	}

	// 清理
	if (!pCase->OnUninitialize(strErrorMessage))
		pti->pStressMan->SetThreadState(pti->nIndex, StressCase::error, strErrorMessage);
	else
		pti->pStressMan->SetThreadState(pti->nIndex, StressCase::ready);

exit_pos:
	InterlockedDecrement(&pti->pStressMan->m_lives);
	return 0;
}

#define WM_USER_ALL_FINISHED	(WM_USER+1)

UINT StressMan::MonitorThreadBody( LPVOID pParam )
{
	ThreadInfo* pti = (ThreadInfo*)pParam;
	for(;;)
	{
		// 如果所有的用例线程都已结束，就通知主窗口做相应的处理
		if (pti->pStressMan->m_lives <= 0)
		{
			CWnd* pWnd = AfxGetMainWnd();
			pWnd->PostMessage(WM_USER_ALL_FINISHED);
			break;
		}
		Sleep(100);
	}	
	return 0;
}

bool StressMan::ReportSuite(LPCTSTR strPath)
{
	TRY
	{
		SYSTEMTIME st; TCHAR buf[100]; DWORD dw = 100;
		GetLocalTime(&st);
		GetComputerName(buf, &dw);
		CString cs;
		CStdioFile file(strPath, 
			CFile::typeText | CFile::modeWrite | CFile::modeCreate );		
	
		file.WriteString(_T("\n=======================\n"));
		file.WriteString(_T("  StressMark 测试报告\n"));
		file.WriteString(_T("=======================\n"));
		cs.Format(_T("\n测试机: %s\n"), buf);
		file.WriteString(cs);
		cs.Format(_T("报告时间: %04d-%02d-%02d %02d:%02d\n"), 
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
		file.WriteString(cs);		
		file.WriteString(_T("\n"));
		file.WriteString(_T("序号\t用例名称\t线程\t计划重复次数"
							"\t已执行次数\t平均每次耗时(秒)\t当前状态\n"));
		file.WriteString(_T("----------------------------------"
							"----------------------------------"
							"------------------------------------------\n"));

		int nIndex = 0;
		for(int i = 0; i < m_nTestSuiteArraySize; i++)
		{
			for(int j = 0; j < g_arrTestSuite[i].m_nClients; j++)
			{
				StressCase* p = (StressCase*)m_arrCaseObjects.GetAt(nIndex);
				cs.Format(_T("%03d\t%s\t%03d\t%d\t%d\t%.3f\t%s\n"), 
					nIndex + 1, 
					g_arrTestSuite[i].m_strName, 
					j + 1, 
					g_arrTestSuite[i].m_nTimes, 
					p->m_nFinishedTimes,
					(p->m_nFinishedTimes != 0) ? p->m_dTime / 
						(double)p->m_nFinishedTimes : 0.0,
					p->m_strState);
				file.WriteString(cs);
				nIndex++;
			}
		}	
		file.WriteString(_T(""));
		file.Close();
		cs.Format(_T("notepad %s"), strPath);
		WinExec(cs, SW_SHOW);
	}
	CATCH_ALL(e)
	{
		e->ReportError();
		return false;
	}
	END_CATCH_ALL

	return true;
}