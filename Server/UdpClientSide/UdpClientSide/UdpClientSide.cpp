// UdpClientSide.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>

#define PORT 10005

struct UdpHeartPack {
    char UDPData[16] ;
};

int _tmain(int argc, _TCHAR* argv[])
{
	struct UdpHeartPack udpPack;
	udpPack.UDPData[0] = 'h';
	udpPack.UDPData[1] = 'e';
	udpPack.UDPData[2] = 'l';
	udpPack.UDPData[3] = 'l';
	udpPack.UDPData[4] = 'o';
	udpPack.UDPData[5] = ' ';
	udpPack.UDPData[6] = 'w';
	udpPack.UDPData[7] = 'o';
	udpPack.UDPData[8] = 'r';
	udpPack.UDPData[9] = 'l';
	udpPack.UDPData[10] = 'd';
	udpPack.UDPData[11] = '\0';
	char *pPack = (char *) &udpPack;

	WSADATA ws;
	SOCKET clientSocket;
	SOCKADDR_IN serverAddr;
	bool fBroadcast = true;

	char sendBuffer[MAX_PATH];
	int nCount = 0;

	// Init windows socket
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
	{
		printf_s("Can't initiates windows socket!Program stop. \n");
		return -1;
	}

	clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
	setsockopt (clientSocket, SOL_SOCKET,SO_BROADCAST, (CHAR *)&fBroadcast, sizeof(bool));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	serverAddr.sin_port = htons(PORT);

	while (true)
	{
		Sleep(1000);
		sprintf_s(sendBuffer, "Message %d is: ok", nCount++);
		sendto(clientSocket, pPack, strlen(sendBuffer), 0, (SOCKADDR *)&serverAddr, sizeof(SOCKADDR_IN));
		printf_s("%s\n", sendBuffer);
	}

	closesocket(clientSocket);
	WSACleanup();
	return 0;
}

