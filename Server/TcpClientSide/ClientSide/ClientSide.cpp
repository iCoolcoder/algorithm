// ClientSide.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>

using namespace std;

#define PORT 10003
#define IP_ADDRESS "127.0.0.1"

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA ws;
	SOCKET clientSocket;
	struct sockaddr_in serverAddr;
	int ret = 0;
	int addrLen = 0;
	HANDLE hThread = NULL;
	char sendBuffer[MAX_PATH];

	// Init windows socket
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
	{
		cout << "Init windows socket failed: " << GetLastError() << endl;
		return -1;
	}

	// Create socket
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		cout << "created socket failed: " << GetLastError() << endl;
		return -1;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
	memset(serverAddr.sin_zero, 0x00, 8);

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (ret == SOCKET_ERROR)
	{
		cout << "connect error: " << GetLastError() << endl;
		return -1;
	}
	else
	{
		cout << "connect successfully" << endl;
	}

	while (true)
	{
		cin.getline(sendBuffer, sizeof(sendBuffer));
		ret = send(clientSocket, sendBuffer, (int)strlen(sendBuffer), 0);
		if (ret == SOCKET_ERROR)
		{
			cout << "send info error: " << GetLastError() << endl;
			break;
		}
	}

	closesocket(clientSocket);
	WSACleanup();
	return 0;
}

