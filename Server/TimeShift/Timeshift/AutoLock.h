#pragma once
#include <Windows.h>

class Lockable
{
private:
	CRITICAL_SECTION m_section;
public:
	Lockable(void)
	{
		InitializeCriticalSection(&m_section);
	}
	~Lockable(void)
	{
		DeleteCriticalSection(&m_section);
	}
	void Lock()
	{
		EnterCriticalSection(&m_section);
	}
	void Unlock()
	{
		LeaveCriticalSection(&m_section);
	}
};

class SRWLockable
{
private:
	SRWLOCK m_srwlock;
public:
	SRWLockable(void)
	{
		InitializeSRWLock(&m_srwlock);
	}
	~SRWLockable(void);
	void LockExclusive()
	{
		AcquireSRWLockExclusive(&m_srwlock);
	}
	void LockShared()
	{
		AcquireSRWLockShared(&m_srwlock);
	}
	void UnlockExclusive()
	{
		ReleaseSRWLockExclusive(&m_srwlock);
	}
	void UnlockShared()
	{
		ReleaseSRWLockShared(&m_srwlock);
	}
};


class AutoLock
{
private:
	Lockable *pLock;
public:
	AutoLock(Lockable *lock) : pLock(lock)
	{
		pLock->Lock();
	}
	~AutoLock()
	{
		pLock->Unlock();
	}
};