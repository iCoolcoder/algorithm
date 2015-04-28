#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <map>
#include <iostream>

#include <event.h>
#include "servicepb.h"



class Connection
{
public:
    int fd;
    struct event ev;
    char *r;
    char *w;
    ::google::protobuf::Message* rsp;
    char remote_ip[16];
    uint16_t remote_port;
    char local_ip[16];
    uint16_t local_port;
    void *bind;
};

class MyRpcServer
{
public:
    MyRpcServer(struct event_base *main_base, uint16_t port);
    ~MyRpcServer();
    bool Start();
    bool RegisterServer(google::protobuf::Service *rpcService);
private:
    static void OnAccept(const int fd, const short which, void *arg);
    static void OnEvent(const int fd, const short which, void *arg);
    int SetSockNBlock(int fd, bool on);
    event_base *GetMainBase();
    Connection *GetConns();
    std::map<std::string, const ::google::protobuf::MethodDescriptor *> & GetRpcCallMap();
    ::google::protobuf::Service * GetService(std::string serviceName);
private:
    struct sockaddr_in _server;
    int _listeningFd;
    struct event _evListener;
    struct event_base *_mainBase;
    Connection *_conns;
    std::map<std::string, const ::google::protobuf::MethodDescriptor *> _rpcCallMap;
    std::map<std::string, ::google::protobuf::Service *> _rpcServices;
};



