// ServerClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>

using namespace std;

#define PORT 10004
#define IP_ADDRESS "127.0.0.1"

DWORD WINAPI clientThread(LPVOID lpParameter)
{
	SOCKET clientSocket = (SOCKET)lpParameter;
	int ret = 0;
	char RecvBuffer[MAX_PATH];
	
	while (true)
	{
		memset(RecvBuffer, 0x00, sizeof(RecvBuffer));
		ret = recv(clientSocket, RecvBuffer, MAX_PATH, 0);
		if (ret == 0 || ret == SOCKET_ERROR)
		{
			cout << "client exit" << endl;
			break;
		}
		cout << "recived message:" << RecvBuffer << endl;
	}
	
	return 0;
}



int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA ws;
	SOCKET serverSocket, clientSocket;
	struct sockaddr_in localAddr, clientAddr;
	int ret = 0;
	int addrLen = 0;
	HANDLE hThread = NULL;

	// Init windows socket
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
	{
		cout << "Init windows socket failed::" << GetLastError() << endl;
		return -1;
	}

	// Create socket
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET)
	{
		cout << "Create socket failed::" << GetLastError() << endl;
		return -1;
	}

	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
	localAddr.sin_port = htons(PORT);
	memset(localAddr.sin_zero, 0x00, 8);

	// Bind socket
	ret = bind(serverSocket, (struct sockaddr*)&localAddr, sizeof(localAddr));
	if (ret != 0)
	{
		cout << "Bind socket failed::" << GetLastError() << endl;
		return -1;
	}

	ret = listen(serverSocket, 10);
	if (ret != 0)
	{
		cout << "Listen socket failed::" << GetLastError() << endl;
		return -1;
	}

	cout << "Server start successfully" << endl;

	while (true)
	{
		addrLen = sizeof(clientAddr);
		clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			cout << "Accept failed::" << GetLastError() << endl;
			break;
		}
		cout<<"client connection::"<<inet_ntoa(clientAddr.sin_addr)<<":"<<clientAddr.sin_port<<endl;
          
        hThread = CreateThread(NULL, 0, clientThread, (LPVOID)clientSocket, 0, NULL);
        if ( hThread == NULL )
        {
            cout<<"Create Thread Failed!"<<endl;
            break;
        }
  
          CloseHandle(hThread);
	}
	
	closesocket(serverSocket);
    closesocket(clientSocket);
    WSACleanup();
	return 0;
}

