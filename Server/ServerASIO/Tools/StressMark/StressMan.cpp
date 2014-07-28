////////////////////////////////////////////////////////////////////////////
//
// StressMan.cpp - StressMark �������в����������̣߳�����
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

	// ͳ�Ʋ��������ж����֣����������õ��߳���Ϊ
	// ÿ�ֲ��������������ɶ���ʵ��
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
		AfxMessageBox(_T("δ�����κβ�������"));
		return false;
	}
	m_arrThreadInfo = new ThreadInfo[m_arrCaseObjects.GetSize()];
	if (m_arrThreadInfo == NULL)
	{
		AfxMessageBox(_T("�ڴ治��"));
		return false;
	}

	// ��ʾ�����߳���Ϣ
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	
	m_list.InsertColumn(0, _T("���"), LVCFMT_LEFT, 40);
	m_list.InsertColumn(1, _T("����������"), LVCFMT_LEFT, 130);
	m_list.InsertColumn(2, _T("�߳�"), LVCFMT_LEFT, 40);
	m_list.InsertColumn(3, _T("�ƻ�ִ�д���"), LVCFMT_RIGHT, 90);
	m_list.InsertColumn(4, _T("��ִ�д���"), LVCFMT_RIGHT, 80);
	m_list.InsertColumn(5, _T("ƽ��ÿ�κ�ʱ(��)"), LVCFMT_RIGHT, 120);
	m_list.InsertColumn(6, _T("��ǰ״̬"), LVCFMT_LEFT, 300);

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
	// ��ԭ״̬�� ready �� error ʱ��
	// ����������״̬Ϊ ready �� stopping
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

	// ����ͬ���¼�
	m_event.ResetEvent();

	// �̻߳��
	m_lives = 0;

	// �������в��������߳�
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

	// ���������߳�
	m_MonitorThreadInfo.pStressMan = this;
	m_MonitorThreadInfo.nIndex = 0;
	m_MonitorThreadInfo.nTimes = 0;
	m_pMonitorThread = AfxBeginThread(StressMan::MonitorThreadBody, &m_MonitorThreadInfo);	

	// ����ͬ���¼���ͬʱ�������������߳�
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
	
	// ��ʼ��
	if (!pCase->OnInitialize(strErrorMessage))
	{
		pti->pStressMan->SetThreadState(pti->nIndex, StressCase::error, strErrorMessage);
		goto exit_pos;
	}	

	// ���������б��
	pti->pStressMan->SetThreadState(pti->nIndex, StressCase::running);
	
	// �ȴ�ͬ���¼�
	WaitForSingleObject(pti->pStressMan->m_event.m_hObject, 60 * 1000l);

	// ����
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

		// ˢ����ִ�д�����ִ����ʱ��
		pti->pStressMan->SetThreadFinishedTimes(pti->nIndex, i+1);
		dTime = pCase->m_dTime + 
			((double)c2.QuadPart - (double)c1.QuadPart) / (double)freq.QuadPart;
		pti->pStressMan->SetThreadTime(pti->nIndex, dTime);

		// �������ã���Ϣһ��ʱ����
		if (pti->nInterval)	Sleep(pti->nInterval);
	}

	// ����
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
		// ������е������̶߳��ѽ�������֪ͨ����������Ӧ�Ĵ���
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
		file.WriteString(_T("  StressMark ���Ա���\n"));
		file.WriteString(_T("=======================\n"));
		cs.Format(_T("\n���Ի�: %s\n"), buf);
		file.WriteString(cs);
		cs.Format(_T("����ʱ��: %04d-%02d-%02d %02d:%02d\n"), 
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
		file.WriteString(cs);		
		file.WriteString(_T("\n"));
		file.WriteString(_T("���\t��������\t�߳�\t�ƻ��ظ�����"
							"\t��ִ�д���\tƽ��ÿ�κ�ʱ(��)\t��ǰ״̬\n"));
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