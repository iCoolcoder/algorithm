#ifndef __DXTHREAD_H__  
#define __DXTHREAD_H__  

#ifdef _WIN32 
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>  
#define DX_CALLBACK WINAPI  
typedef HANDLE DXHANDLE;
#else
#include <pthread.h>  
#define DX_CALLBACK  
typedef pthread_t DXHANDLE;
#endif  

class CDXThread
{
public:
    CDXThread();
    virtual ~CDXThread();
    virtual int Run();
    bool Start();
    bool Join();
    bool Detach();

private:
    static void* DX_CALLBACK RunThread(void* pParam);
    DXHANDLE m_hThread;
    void* m_pParam;
    unsigned long m_nRet;
protected:
    bool m_bStart;
    int m_nState;
};

#endif /* DXThread.h */
