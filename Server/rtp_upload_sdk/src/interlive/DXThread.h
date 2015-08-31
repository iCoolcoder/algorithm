#ifndef __DXTHREAD_H__  
#define __DXTHREAD_H__  

#ifdef _WIN32 
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>  
#include <WinSock2.h>
#include <WS2tcpip.h>
#define DX_CALLBACK WINAPI  
typedef HANDLE DXHANDLE;
#elif
#include <pthread.h>  
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#define DX_CALLBACK  
typedef pthread_t DXHANDLE;
#endif  

#include "rtp_trans/rtp_trans_manager.h"
#include <string>
#include <iostream>
using namespace std;

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

class Sender : public CDXThread
{
public:
    Sender(string ip, string port, RTPTransManager *rtp_trans_mgr) :
        _fd(0),
        _ip(ip),
        _port(port),
        _rtp_trans_mgr(rtp_trans_mgr)
    {

    }

    int init()
    {
        _fd = socket(AF_INET, SOCK_DGRAM, 0);
        return 0;
    }

    int Run()
    {
        init();
        while (1)
        {
            // _trans_mgr->get();
            
            char buf[65536];
            int buf_len = _rtp_trans_mgr->get_one_rtp_rtcp(buf, 65536);
            
            struct sockaddr_in client_addr;
            socklen_t size = sizeof(struct sockaddr);
            client_addr.sin_family = AF_INET;
            client_addr.sin_addr.s_addr = inet_addr(_ip.c_str());
            client_addr.sin_port = htons(atoi(_port.c_str()));
            int len = sendto(_fd, buf, buf_len, 0, (struct sockaddr *)&client_addr, size);
            //if (len < 0)
            //    cout << "send error." << len << endl;
            //else
            //    cout << len << endl;

            // 10ms
#ifdef _WIN32
            Sleep(10);
#elif
            usleep(10000);
#endif
        }
        return 0;
    }
private:
    int _fd;
    string _ip;
    string _port;
    RTPTransManager *_rtp_trans_mgr;
};

class Receiver : public CDXThread
{

public:
    Receiver(string ip, string port, RTPTransManager *rtp_trans_mgr) :
        _fd(0),
        _ip(ip),
        _port(port),
        _rtp_trans_mgr(rtp_trans_mgr)
    {

    }

    int init()
    {
        _client_addr.sin_family = AF_INET;
        _client_addr.sin_port = htons(atoi(_port.c_str()));
        _client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        _fd = socket(AF_INET, SOCK_DGRAM, 0);
        bind(_fd, (struct sockaddr*)&_client_addr, sizeof(_client_addr));
        return 0;
    }

    int Run()
    {
        init();
        while (1)
        {
            char buf[65536];
            
            socklen_t size = sizeof(struct sockaddr);
            int len = recvfrom(_fd, buf, 65536, 0, (struct sockaddr *)&_client_addr, &size);

            if (len > 0)
            {
                cout << "Recv: " << len << endl;
                _rtp_trans_mgr->put_rtcp(buf, len);
            }
            else
            {
                cout << "Recv error." << endl;
            }
#ifdef _WIN32
            Sleep(10);
#elif
            usleep(10000);
#endif
        }
        return 0;
    }
private:
    int _fd;
    string _ip;
    string _port;
    struct sockaddr_in _client_addr;
    RTPTransManager *_rtp_trans_mgr;
};

#endif /*DXThread.h*/
