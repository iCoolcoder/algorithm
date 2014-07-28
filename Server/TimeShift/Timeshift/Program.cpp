// SafeCircularCache.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "SafeCircularCache.h"
#include "UdpRecvSocket.h"
#include "TcpRecvSocket.h"
#include "GlobalConfigration.h"
#include <string>

using namespace std;

string fn_(const string& s)
{
	return s;
}


int init()
{
	GlobalConfigration::sharedCache = new lru_cache_using_std_s<string, string>(fn_, GlobalConfigration::sharedCacheSize);
	GlobalConfigration::pBufMgr = new MemPool::CMemoryPool();

	WSADATA ws;
	// Init windows socket
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
	{
		cout << "Init windows socket failed: " << GetLastError() << endl;
		WSACleanup();
		return -1;
	}

	return 1;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (!init())
	{
		return -1;
	}
	
	// Udp
	UdpRecvSocket sock("123", 10002, "127.0.0.1");
	sock.Start();
	UdpRecvSocket sock1("124", 10003, "127.0.0.1");
	sock1.Start();
	// Tcp
	TcpRecvSocket sock2(10007);
	sock2.Start();

	system("pause");
	return 0;
}

