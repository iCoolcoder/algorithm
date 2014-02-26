// LockDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "LockDLL.h"

CriticalLock::CriticalLock()
{
	InitializeCriticalSection(&g_cs);
}

CriticalLock::~CriticalLock(void)
{
	DeleteCriticalSection(&g_cs);
}

void CriticalLock::Lock()
{
	EnterCriticalSection(&g_cs);
}

void CriticalLock::UnLock()
{
	LeaveCriticalSection(&g_cs);
}

SRWLock::SRWLock()
{
	InitializeSRWLock(&g_SRWLock);
}

SRWLock::~SRWLock()
{
	
}

void SRWLock::ExclusiveLock()
{
	AcquireSRWLockExclusive(&g_SRWLock);
}

void SRWLock::ReleaseExclusiveLock()
{
	ReleaseSRWLockExclusive(&g_SRWLock);
}

void SRWLock::SharedLock()
{
	AcquireSRWLockShared(&g_SRWLock);
}

void SRWLock::ReleaseSharedLock()
{
	ReleaseSRWLockShared(&g_SRWLock);
}

ConditionSRW::ConditionSRW()
{
	
}

ConditionSRW::~ConditionSRW()
{
}

void ConditionSRW::Sleep(PSRWLOCK pSRWLock, DWORD dwMillseconds, ULONG Flags)
{
	SleepConditionVariableSRW(&g_cv, pSRWLock, dwMillseconds, Flags);
}

void ConditionSRW::WakeOne()
{
	WakeConditionVariable(&g_cv);
}

void ConditionSRW::WakeAll()
{
	WakeAllConditionVariable(&g_cv);
}



/**************************************************************************************/

