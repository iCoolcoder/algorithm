#include <iostream>
#include "rpc_channel.h"

using namespace echo;

using namespace std;

int main(int argc, char *argv[])
{
    MyRpcChannel *channel = new MyRpcChannel("127.0.0.1", 9999);
    if (!channel->Connect())
    {
        printf("connect to server failed, abort\n");
        exit(-1);
    }
    EchoService::Stub stub(channel);
    EchoRequest request;
    EchoResponse response;
    request.set_message("hello");
    stub.Echo(NULL, &request, &response, NULL);
	return 0;
}
