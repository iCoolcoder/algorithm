#include "DXThread.h"  
#include <stddef.h>

//int
//util_set_nonblock(int fd, int on)
//{
//    int flags = 1;
//
//    if (fd > 0) {
//        flags = fcntl(fd, F_GETFL);
//        if (-1 == flags)
//            return -1;
//        if (on)
//            flags |= O_NONBLOCK;
//        else
//            flags &= ~O_NONBLOCK;
//        if (0 != fcntl(fd, F_SETFL, flags))
//            return -2;
//    }
//    return 0;
//}

CDXThread::CDXThread()
{
    m_hThread = NULL;
    m_pParam = NULL;
    m_nRet = 0;
    m_bStart = false;
    m_nState = 0;
}

CDXThread::~CDXThread()
{
    if (m_bStart && (m_nState == 0))
    {
        Join();
    }
}

int CDXThread::Run()
{
    return 0;
}

bool CDXThread::Start()
{
    if (true == m_bStart)
        return true;

    bool bOK = true;
    m_bStart = true;
#ifdef _WIN32 
    if (NULL ==
        (
        m_hThread = CreateThread(NULL, 0,
        (unsigned long(DX_CALLBACK*)(void*))&RunThread, this, 0, NULL)
        )
        )
    {
        bOK = false;
    }
#elif  
    if (0 != pthread_create(&m_hThread, NULL, RunThread, this))
    {
        bOK = false;
    }
#endif  

    if (!bOK)
    {
        m_bStart = false;
        return false;
    }

    return true;
}

bool CDXThread::Join()
{
    if (!m_bStart)
        return false;

    if (m_nState == 1)
        return false;

    bool ret;
    m_bStart = false;

#ifdef _WIN32  

    while (1)
    {
        if (FALSE == GetExitCodeThread(m_hThread, &m_nRet))
        {
            ret = false;
            break;
        }
        else
        {
            if (m_nRet == STILL_ACTIVE)
            {
                Sleep(100);
                continue;
            }
            CloseHandle(m_hThread);
            ret = true;
            break;
        }
    }

#endif  

#ifndef _WIN32  
    ret = (0 == pthread_join(m_hThread, (void **)&m_nRet));
#endif  

    m_hThread = NULL;
    return ret;
}

bool CDXThread::Detach()
{
    if (!m_bStart)
        return false;

    if (m_nState == 1)
        return false;

    m_nState = 1;
#ifdef _WIN32
    return (TRUE == CloseHandle(m_hThread));
#elif  
    return (0 == pthread_detach(m_hThread));
#endif  

}

void* DX_CALLBACK CDXThread::RunThread(void* pParam)
{
    CDXThread* pThis = (CDXThread*)pParam;
    int nRet = pThis->Run();
    return (void *)nRet;
}
