#pragma once
#include <iostream>
#include "Socket.h"

class TcpRecvSocket
{
public:
	TcpRecvSocket(int port);
	~TcpRecvSocket(void);
	void Start();
private:
	int m_port;
};

