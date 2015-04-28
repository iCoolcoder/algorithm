#include "rpc_server.h"

using namespace std;

MyRpcServer::MyRpcServer(struct event_base *mainBase, uint16_t port)
    : _mainBase(mainBase)
{
    _server.sin_family = AF_INET;
    _server.sin_port = htons(port);
    _server.sin_addr.s_addr = INADDR_ANY;
    bzero(&(_server.sin_zero), 8);
    RegisterServer(new echo::EchoService_Impl());
}


MyRpcServer::~MyRpcServer()
{
}

bool MyRpcServer::Start() {
    if ((_listeningFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return false;
    }

    int val = 1;
    if (-1 == setsockopt(_listeningFd, SOL_SOCKET, SO_REUSEADDR,
        (void *)&val, sizeof(val))) {
        return false;
    }
    fcntl(_listeningFd, F_SETFD, FD_CLOEXEC);
    if (0 != SetSockNBlock(_listeningFd, true)) {
        return false;
    }

    if (bind(_listeningFd, (struct sockaddr*)&_server, sizeof(_server)) < 0) {
        return false;
    }

    if (listen(_listeningFd, 5120) < 0) {
        return false;
    }

    _conns = new Connection[1000];
    for (int i = 0; i < 1000; i++) {
        _conns[i].fd = -1;
    }

    event_set(&_evListener, _listeningFd, EV_READ | EV_PERSIST, OnAccept, (void *)this);
    event_base_set(_mainBase, &_evListener);
    event_add(&_evListener, 0);
}

int MyRpcServer::SetSockNBlock(int fd, bool on)
{
    int flags = 1;

    if (fd > 0) {
        flags = fcntl(fd, F_GETFL);
        if (-1 == flags)
            return -1;
        if (on)
            flags |= O_NONBLOCK;
        else
            flags &= ~O_NONBLOCK;
        if (0 != fcntl(fd, F_SETFL, flags))
            return -2;
    }
    return 0;
}

event_base *MyRpcServer::GetMainBase()
{
    return _mainBase;
}

Connection *MyRpcServer::GetConns()
{
    return _conns;
}


void MyRpcServer::OnAccept(const int fd, const short which, void *arg)
{
    MyRpcServer *rpcServer = (MyRpcServer *)arg;
    int newFd = -1;
    struct sockaddr_in sIn;
    socklen_t len = sizeof(struct sockaddr_in);

    memset(&sIn, 0, len);
    newFd = accept(fd, (struct sockaddr *) &sIn, &len);
    if (-1 == newFd) {
        return;
    }
    rpcServer->SetSockNBlock(newFd, true);

    Connection *c = rpcServer->GetConns() + newFd;
    c->fd = newFd;
    event_set(&c->ev, newFd, EV_READ | EV_PERSIST, OnEvent, (void *)c);

    c->r = new char[1024];
    c->w = new char[512 * 1024];
    c->bind = (void *)rpcServer;
    event_base_set(rpcServer->GetMainBase(), &c->ev);
    event_add(&c->ev, 0);
}

std::map<std::string, const ::google::protobuf::MethodDescriptor *> & MyRpcServer::GetRpcCallMap()
{
    return _rpcCallMap;
}

::google::protobuf::Service * MyRpcServer::GetService(string serviceName)
{
    return _rpcServices[serviceName];
}

void MyRpcServer::OnEvent(const int fd, const short which, void *arg)
{
    Connection *c = (Connection *)arg;
    MyRpcServer *rpcServer = (MyRpcServer *)c->bind;
    if (which & EV_READ) {
        char buffer[1024];
        int size = read(c->fd, buffer, 1024);
        if (size == 0) {
            return;
        }
        buffer[size] = '\0';
        std::cout << buffer << std::endl;
        char * pSM = strtok(buffer, "|");
        std::string sm(pSM);
        std::cout << sm << std::endl;
        const google::protobuf::MethodDescriptor *pMethodDes = rpcServer->GetRpcCallMap()[sm];
        ::google::protobuf::Service * pService = rpcServer->GetService(pMethodDes->service()->full_name());
        c->rsp = pService->GetResponsePrototype(pMethodDes).New();
        pService->CallMethod(pMethodDes, NULL, NULL, c->rsp, NULL);

        event_del(&c->ev);
        event_set(&c->ev, c->fd, EV_READ | EV_WRITE | EV_PERSIST,
            OnEvent, arg);
        event_base_set(rpcServer->GetMainBase(), &c->ev);
        event_add(&c->ev, 0);
    }
    else {
        std::string *output = new std::string;
        c->rsp->SerializeToString(output);
        send(c->fd, output->c_str(), output->length(), 0);

    }
}

bool MyRpcServer::RegisterServer(google::protobuf::Service *rpcService)
{
    const google::protobuf::ServiceDescriptor *pSerDes = rpcService->GetDescriptor();
    _rpcServices[pSerDes->full_name()] = rpcService;
    int methodCnt = pSerDes->method_count();

    for (int i = 0; i < methodCnt; i++) {
        const google::protobuf::MethodDescriptor *pMethodDes = pSerDes->method(i);
        
        const std::string& service_name = pMethodDes->service()->name();
        const std::string& method_name = pMethodDes->name();
        _rpcCallMap[service_name + ":" + method_name] = pMethodDes;
        
    }
    return true;
}
