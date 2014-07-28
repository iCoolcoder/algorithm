#pragma once
#include "GlobalConfigration.h"
#include <iostream>
#include <string>
#include "Socket.h"
#include <Windows.h>

using namespace std;

class UdpRecvSocket
{
public:
	UdpRecvSocket(string channel, int port, string ipAddr);
	~UdpRecvSocket(void);
	void Start();
private:
	string m_channel;
	int m_port;
	string m_ipAddr;
};

