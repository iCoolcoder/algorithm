#ifndef __DXLOCK_H_
#define __DXLOCK_H_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif
#include <pthread.h>
#endif

class CMutex
{
public:
#ifdef _WIN32
    typedef CRITICAL_SECTION Mutex_T;
#elif
    typedef pthread_mutex_t Mutex_T;
    typedef pthread_mutexattr_t Mattr_T;
#endif

public:
    CMutex() 
    {
#ifdef _WIN32
        InitializeCriticalSection(&m_mutex);
#endif
    }

    ~CMutex() 
    {
#ifdef _WIN32
        DeleteCriticalSection(&m_mutex);
#endif
    }

public:
    int Lock()
    {
#ifdef _WIN32
        EnterCriticalSection(&m_mutex);
        return 0;
#elif
        return pthread_mutex_lock(&m_mutex);
#endif
    }

    int Unlock()
    {
#ifdef _WIN32
        LeaveCriticalSection(&m_mutex);
        return 0;
#elif
        return pthread_mutex_unlock(&m_mutex);
#endif
    }

    int Trylock()
    {
#ifdef _WIN32
        return TryEnterCriticalSection(&m_mutex);
#elif
        return pthread_mutex_trylock(&m_mutex);
#endif
    }

    Mutex_T& GetMutex()
    {
        return m_mutex;
    }

private:
    Mutex_T m_mutex;
};

class CMAutoLock 
{
public:
    explicit CMAutoLock(CMutex& mutex) : m_lock(mutex)
    {
        m_lock.Lock();
    }
    ~CMAutoLock()
    {
        m_lock.Unlock();
    }

private:
    CMutex& m_lock;

private:
    CMAutoLock operator=(const CMAutoLock& mutex);
    CMAutoLock(const CMAutoLock& mutex);
};

#endif
