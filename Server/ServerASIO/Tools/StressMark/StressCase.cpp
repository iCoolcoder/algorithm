////////////////////////////////////////////////////////////////////////////
//
// StressCase.cpp - ��������������ʵ�ִ���
//
// created by Wang Yong Gang, 2001-08-07 ( version 1.0 )
// modified by Wang Yong Gang, 2002-02-16 ( version 1.1 )
//
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StressCase.h"

IMPLEMENT_DYNCREATE(StressCase, CObject)

StressCase::StressCase()
{
	SetClientState(ready);
	m_nFinishedTimes = 0;
	m_dTime = 0.0;
}

void StressCase::SetClientState(int nState, LPCTSTR strMessage)
{	
	m_nState = nState;
	switch(nState)
	{
	case ready:		m_strState = _T("��ֹͣ");		break;
	case starting:	m_strState = _T("�����С���");	break;
	case running:	m_strState = _T("����");		break;
	case stopping:	m_strState = _T("ֹͣ�С���");	break;
	case error:		m_strState = _T("������ֹ");	break;		
	}
	if (strMessage != NULL)
	{
		m_strState += ": "; m_strState += strMessage;
	}
}

void StressCase::LogEvent(LPCTSTR strMessage)
{
	TCHAR temp[201];
	LPCTSTR arrString[1];
	HANDLE h; 

	_sntprintf(temp, 200, _T("[%d][%s][%d][��%d��] %s"), 
		m_nID, m_strName, m_nClientID, m_nFinishedTimes + 1, strMessage);
	arrString[0] = temp;
 
    h = RegisterEventSource(NULL, m_strName);
    if (h == NULL) 
        return; 
 
    if (!ReportEvent(h, EVENTLOG_INFORMATION_TYPE, 0, 0,
            NULL, 1, 0, arrString, NULL))
	{
		DWORD err = GetLastError();
	}
 
    DeregisterEventSource(h); 
}