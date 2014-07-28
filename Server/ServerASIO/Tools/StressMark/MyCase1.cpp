
// 演示用的测试用例类

#include "stdafx.h"
#include "StressCase.h"
#include "MyCase1.h"
#include <Windows.h>
#include <iostream>

using namespace std;

IMPLEMENT_DYNCREATE(MyCase1, StressCase)

bool MyCase1::OnInitialize(CString& strErrorMessage)
{
	WSADATA ws;
	// Init windows socket
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
	{
		WSACleanup();
		cout << "Init windows socket failed: " << GetLastError() << endl;
		return false;
	}

	return true;
}

bool MyCase1::OnUninitialize(CString& strErrorMessage)
{
	WSACleanup();
	return true;
}

bool MyCase1::OneStep(CString& strErrorMessage)
{
	// do something...
	SOCKET clientSocket;
	struct sockaddr_in serverAddr;
	int ret = 0;
	int addrLen = 0;
	HANDLE hThread = NULL;
	char sendBuffer[MAX_PATH];
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
		const char *str = "client message";
		memset(sendBuffer, '\0', sizeof(sendBuffer));
		strcpy(sendBuffer, str);
		ret = send(clientSocket, sendBuffer, 260, 0);
		if (ret == SOCKET_ERROR)
		{
			cout << "send info error: " << GetLastError() << endl;
			break;
		}
	}

	closesocket(clientSocket);
	Sleep(1000);
	return true;
}