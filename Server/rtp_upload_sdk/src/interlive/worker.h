#ifndef __WORKER_H__  
#define __WORKER_H__  

#ifdef _WIN32 
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>  
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#endif  

#include "rtp_trans/rtp_trans_manager.h"
#include <string>
#include <iostream>
#include "common/proto.h"
#include "util/buffer.h"
#include "util/log.h"
#include "util/DXThread.h"
using namespace std;

class Sender : public CDXThread
{
public:
    Sender(int fd, string ip, string port, RTPTransManager *rtp_trans_mgr) :
        _fd(fd),
        _ip(ip),
        _port(port),
        _rtp_trans_mgr(rtp_trans_mgr)
    {

    }

    int init()
    {
        return 0;
    }

    int Run()
    {
        init();
        while (1)
        {
            char buf[65536];
            int buf_len = _rtp_trans_mgr->get_one_rtp_rtcp(buf, 65536);
            if (buf_len <= 0) {
                continue;
            }
            
            struct sockaddr_in client_addr;
            socklen_t size = sizeof(struct sockaddr);
            client_addr.sin_family = AF_INET;
            client_addr.sin_addr.s_addr = inet_addr(_ip.c_str());
            client_addr.sin_port = htons(atoi(_port.c_str()));
            int len = sendto(_fd, buf, buf_len, 0, (struct sockaddr *)&client_addr, size);

            /* sleep 1ms */
#ifdef _WIN32
            Sleep(1);
#else
            usleep(1000);
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
    Receiver(int fd, RTPTransManager *rtp_trans_mgr) :
        _fd(fd),
        _rtp_trans_mgr(rtp_trans_mgr),
        _internal_used_buf(NULL)
    {

    }

    ~Receiver()
    {
        buffer_free(_internal_used_buf);
    }

    int init()
    {
        _internal_used_buf = buffer_create_max(65536, 65536);
        return 0;
    }

    int Run()
    {
        init();
        while (1)
        {
            char buf[65536];
            
            socklen_t size = sizeof(struct sockaddr);
            int len = recvfrom(_fd, buf, sizeof(buf), 0,
                (struct sockaddr *)&_client_addr, &size);

            if (len > 0)
            {
                buffer_reset(_internal_used_buf);
                buffer_append_ptr(_internal_used_buf, buf, len);
                proto_header ph;
                decode_header(_internal_used_buf, &ph);
                
                switch (ph.cmd)
                {
                case CMD_RTCP_U2R_PACKET:
                    buffer_eat(_internal_used_buf, sizeof(proto_header));
                    _rtp_trans_mgr->put_rtcp(buffer_data_ptr(_internal_used_buf), 
                        buffer_data_len(_internal_used_buf));
                    break;
                default:
                    break;
                }
            }

            /* sleep 1ms */
#ifdef _WIN32
            Sleep(1);
#else
            usleep(1000);
#endif
        }
        return 0;
    }
private:
    int _fd;
    buffer * _internal_used_buf;
    struct sockaddr_in _client_addr;
    RTPTransManager *_rtp_trans_mgr;
};

class RTPTransTimer : public CDXThread
{
public:
    RTPTransTimer(RTPTransManager *rtp_trans_mgr) :
        _running(false),
        _rtp_trans_mgr(rtp_trans_mgr)
    {

    }
    ~RTPTransTimer()
    {

    }
    int init()
    {
        _running = true;
        return 0;
    }
    void set_state(bool flag)
    {
        _running = flag;
    }
    int Run()
    {
        init();
        while (_running)
        {
            _rtp_trans_mgr->on_timer();
            /* sleep 10ms */
#ifdef _WIN32
            Sleep(10);
#else
            usleep(10000);
#endif
        }
        return 0;
    }
private:
    bool _running;
    RTPTransManager *_rtp_trans_mgr;
};

#endif /* __WORKER_H__ */
