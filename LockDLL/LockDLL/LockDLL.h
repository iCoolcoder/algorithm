#ifdef FUNDLL_EXPORTS
#define FUNDLL_API extern "C" __declspec(dllexport)
#else
#define FUNDLL_API extern "C" __declspec(dllexport)
#endif

// This class is exported from the LockDLL.dll
class CriticalLock {
public:
	CriticalLock();
	~CriticalLock(void);
	void Lock();
	void UnLock();
private:
	CRITICAL_SECTION g_cs;
};

class SRWLock {
public:
	SRWLock();
	~SRWLock();
	void ExclusiveLock();
	void ReleaseExclusiveLock();
	void SharedLock();
	void ReleaseSharedLock();
private:
	SRWLOCK g_SRWLock;
};

class ConditionSRW {
public:
	ConditionSRW();
	~ConditionSRW();
	void Sleep(PSRWLOCK pSRWLock, DWORD dwMillseconds, ULONG Flags);
	void WakeOne();
	void WakeAll();
private:
	SRWLOCK g_SRW;
	CONDITION_VARIABLE g_cv;
};

