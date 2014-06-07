// UdpServerSide.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>

#define PORT 7001

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA ws;
	SOCKET serverSocket;
	SOCKADDR_IN clientAddr, sin;

	int nbSize, nSize;

	char recvBuffer[MAX_PATH];

	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
	{
		printf_s("can't initiate windows socket! Program stop. \n");
		return -1;
	}

	serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(PORT);
	clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(serverSocket, (SOCKADDR FAR *)&clientAddr, sizeof(clientAddr)) != 0)
	{
		printf_s("can't bind socket to local port! program stop. \n");
		return -1;
	}

	while (true)
	{
		memset(recvBuffer, 0x00, sizeof(recvBuffer));
		nSize = sizeof(SOCKADDR_IN);
		if ((nbSize = recvfrom(serverSocket, recvBuffer, MAX_PATH, 0, (SOCKADDR FAR *)&sin, &nSize)) == SOCKET_ERROR)
		{
			printf_s("receive error. \n");
			break;
		}

		printf_s("%s\n", recvBuffer);
	}

	WSACleanup();

	return 0;
}

