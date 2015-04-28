#include "rpc_channel.h"

namespace echo {
    MyRpcChannel::MyRpcChannel(char * ip, uint32_t port)
    {
        _server.sin_family = AF_INET;
        _server.sin_port = htons(port);
        _server.sin_addr.s_addr = inet_addr(ip);
        bzero(&(_server.sin_zero), 8);
    }

    MyRpcChannel::~MyRpcChannel()
    {
    }

    bool MyRpcChannel::Connect() {
        _fd = socket(AF_INET, SOCK_STREAM, 0);
        int ret = connect(_fd, (struct sockaddr *)&_server, sizeof(struct sockaddr));
        if (0 == ret)
            return true;
        else
            return false;
    }

    void MyRpcChannel::CallMethod(const MethodDescriptor* method,
        RpcController* controller,
        const Message* request,
        Message* response,
        Closure* done) {
        const std::string& service_name = method->service()->name();
        const string& method_name = method->name();
        string * output = new string;
        request->SerializeToString(output);
        cout << *output << endl;
        *output = service_name + ":" + method_name + "|" + *output;
        cout << *output << endl;
        send(_fd, output->c_str(), output->length(), 0);
        char buffer[1024];
        int size = recv(_fd, buffer, 1024, 0);
        buffer[size] = '\0';
        cout << buffer << endl;
        if (done)
            done->Run();
    }
}


