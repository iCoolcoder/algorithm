#include <iostream>
#include "rpc_server.h"
#include <event.h>

using namespace std;
using namespace echo;

int main(int argc, char *argv[])
{
    event_base *mainBase = event_base_new();
    if (NULL == mainBase)
    {
        return 1;
    }
    MyRpcServer *pRpcServer =  new MyRpcServer(mainBase, 9999);
    pRpcServer->Start();
    event_base_dispatch(mainBase);
	return 0;
}
