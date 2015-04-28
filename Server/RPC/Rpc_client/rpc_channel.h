#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include "servicepb.h"

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
#include <google/protobuf/io/coded_stream.h>

using namespace ::google::protobuf;
using ::google::protobuf::Closure;
using ::google::protobuf::NewCallback;
using ::google::protobuf::NewPermanentCallback;
using ::google::protobuf::Message;
using ::google::protobuf::MethodDescriptor;
using ::google::protobuf::Mutex;
using ::google::protobuf::MutexLock;
using ::google::protobuf::RpcController;
using ::google::protobuf::io::CodedOutputStream;
using ::google::protobuf::io::CodedInputStream;
using ::google::protobuf::io::ZeroCopyOutputStream;

namespace echo {
    class MyRpcChannel : public ::google::protobuf::RpcChannel
    {
    public:
        MyRpcChannel(char * ip, uint32_t port);
        ~MyRpcChannel();
        bool Connect();
        void CallMethod(const MethodDescriptor* method,
            RpcController* controller,
            const Message* request,
            Message* response,
            Closure* done);
    private:
        struct sockaddr_in _server;
        
        uint32_t _fd;
    };
}


