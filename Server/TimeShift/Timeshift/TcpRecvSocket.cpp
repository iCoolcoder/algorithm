#include "stdafx.h"
#include "TcpRecvSocket.h"


TcpRecvSocket::TcpRecvSocket(int port) :  m_port(port)
{
}


TcpRecvSocket::~TcpRecvSocket(void)
{
}

VOID CALLBACK TcpIOCPThread(DWORD dwErrorCode, DWORD dwBytesTrans, LPOVERLAPPED lpOverlapped)
{
	if (NULL == lpOverlapped)
	{
		SleepEx(20, TRUE);
		return;
	}

	SOCKETOVERLAPPED *pOL = (SOCKETOVERLAPPED *)lpOverlapped;

	switch (pOL->m_opType)
	{
	case 0:
		{
			cout << "one connection." << endl;
			LPSOCKADDR addrHost = NULL;      //server addr
			LPSOCKADDR addrClient = NULL;     //client addr
			int lenHost = 0;
			int lenClient = 0;
			CGRSMsSockFun sockFuns(pOL->m_clientSocket);
			sockFuns.GetAcceptExSockaddrs(pOL->m_pBuf, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, (LPSOCKADDR*)&addrHost, &lenHost, (LPSOCKADDR*)&addrClient, &lenClient);
		
			int nRet = ::setsockopt(pOL->m_clientSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char *)&pOL->m_serverSocket, sizeof(SOCKET));
			cout << pOL->m_pBuf << endl;
		
			sockFuns.DisconnectEx(pOL->m_clientSocket, &pOL->m_ol, TF_REUSE_SOCKET, 0);
			break;
		}
	case 1:
		{
		break;
		}
	default:
		break;
	}
}


void TcpRecvSocket::Start()
{
	SOCKADDR_IN saServer = { 0 };
	
	// Create listening socket
	SOCKET serverSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == serverSocket) 
	{
		printf("\nError occurred while opening socket: %d.", WSAGetLastError());
		return;
	}
	else
	{
		printf("\nWSASocket() successful.");
	}

	// Push into thread pool
	BindIoCompletionCallback((HANDLE)serverSocket, TcpIOCPThread, 0);
	
	ZeroMemory((char *)&saServer, sizeof(saServer));
	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = INADDR_ANY;
	saServer.sin_port = htons(10003);

	if (SOCKET_ERROR == ::bind(serverSocket, (LPSOCKADDR)&saServer, sizeof(SOCKADDR_IN))) 
	{
		closesocket(serverSocket);
		printf("\nError occurred while binding.");
		return;
	}
	else
	{
		printf("\nbind() successful.");
	}


	if (SOCKET_ERROR == listen(serverSocket, SOMAXCONN))
	{
		closesocket(serverSocket);
		printf("\nError occurred while listening.");
		return;
	}
	else
	{
		printf("\nlisten() successful.");
	}

	SOCKET clientSocket;
	SOCKETOVERLAPPED *pOL;
	CGRSMsSockFun sockFuns(serverSocket);
	for (UINT i = 0; i < 100; i++)
	{
		clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		BindIoCompletionCallback((HANDLE)clientSocket, TcpIOCPThread, 0);
		pOL = new SOCKETOVERLAPPED;
		ZeroMemory(pOL, sizeof(SOCKETOVERLAPPED));
		pOL->m_opType = 0; // AcceptEx operation
		pOL->m_serverSocket = serverSocket;
		pOL->m_clientSocket = clientSocket;
		BYTE *pBuf = new BYTE[256];
		ZeroMemory(pBuf, 256 * sizeof(BYTE));
		pOL->m_pBuf = pBuf;

		if (FALSE == sockFuns.AcceptEx(serverSocket, clientSocket, pBuf,
			0, // set recv buffer 0 in order to make acceptEx return directly. This is helpful to stop service attack
			256, 256, NULL, (LPOVERLAPPED)pOL))
		{
			if (ERROR_IO_PENDING == WSAGetLastError())
				printf("\nAcceptEx() successful.");
			else
				printf("\nAcceptEx() Error: %d", WSAGetLastError());
		}
		else
		{
			printf("\nAcceptEx() successful.");
		}
	}

	printf("\nTcp listening.");
}