#include "stdafx.h"
#include "UdpRecvSocket.h"

UdpRecvSocket::UdpRecvSocket(string channel, int port, string ipAddr) : m_channel(channel), m_port(port), m_ipAddr(ipAddr)
{

}

UdpRecvSocket::~UdpRecvSocket(void)
{

}

VOID CALLBACK IOCPThread(DWORD dwErrorCode, DWORD dwBytesTrans, LPOVERLAPPED lpOverlapped)
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
			break;
		}
	case 1:
		{
			cout << "one connection" << endl;
			LPWSABUF pBuf = (LPWSABUF)pOL->m_pBuf;
			if (pBuf->len != 0)
				cout << pBuf->buf << endl;
			
			string s(pBuf->buf);

			GlobalConfigration::sharedCache->operator()(s);
			
			SOCKETOVERLAPPED *pNewOL = new SOCKETOVERLAPPED;
			ZeroMemory(pNewOL, sizeof(SOCKETOVERLAPPED));

			pNewOL->m_serverSocket = pOL->m_serverSocket;
			pNewOL->m_pBuf = pOL->m_pBuf;
			ZeroMemory(pBuf->buf, 256 * sizeof(CHAR));
			pNewOL->m_opType = 1;
			pNewOL->m_addrLen = sizeof(SOCKADDR_IN);
			delete pOL;
			DWORD recvedBytes = 0;
			DWORD flag = 0;

			int ret = WSARecvFrom(pNewOL->m_serverSocket, (LPWSABUF)pNewOL->m_pBuf, 1, &recvedBytes, &flag, &pNewOL->m_fromAddr, &pNewOL->m_addrLen, (LPOVERLAPPED)pNewOL, 0);
			if (ret == SOCKET_ERROR)
			{
				DWORD errorCode = WSAGetLastError();
				if (errorCode != WSA_IO_PENDING)
				{
					// shutdown everything since error happened.
					closesocket(pNewOL->m_serverSocket);
					delete pNewOL;
				}
			}
			break;
		}
	default:
		break;
	}
}


void UdpRecvSocket::Start()
{
	SOCKET serverSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (serverSocket != INVALID_SOCKET)
	{
		SOCKADDR_IN sockAddr = { 0 };
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_addr.s_addr = INADDR_ANY;
		sockAddr.sin_port = htons(m_port);

		bool socketOptionValue = true;
		int ret = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&socketOptionValue, sizeof(socketOptionValue));
		if (ret != SOCKET_ERROR)
		{
			socketOptionValue = FALSE;
			DWORD outSize = 0;
			ret = WSAIoctl(serverSocket, SIO_UDP_CONNRESET, &socketOptionValue, sizeof(socketOptionValue), NULL, 0, &outSize, NULL, NULL);
			if (ret != SOCKET_ERROR)
			{
				ret = ::bind(serverSocket, (SOCKADDR *)&sockAddr, sizeof(sockAddr));
				if (ret != SOCKET_ERROR)
				{
					BindIoCompletionCallback((HANDLE)serverSocket, IOCPThread, 0);
				}
			}
		}

	}
	else
	{
		closesocket(serverSocket);
		serverSocket = INVALID_SOCKET;
	}

	SOCKETOVERLAPPED *pOL = new SOCKETOVERLAPPED;
	if (pOL)
	{
		ZeroMemory(pOL, sizeof(SOCKETOVERLAPPED));
		pOL->m_serverSocket = serverSocket;


		WSABUF *pBuf = new WSABUF;
		pBuf->buf = new CHAR[256];
		pBuf->len = 256;
		ZeroMemory(pBuf->buf, 256 * sizeof(CHAR));
		pOL->m_pBuf = (LPVOID)pBuf;
		pOL->m_opType = 1;
		pOL->m_addrLen = sizeof(SOCKADDR_IN);

		DWORD recvedBytes = 0;
		DWORD flag = 0;
		int ret = WSARecvFrom(serverSocket, pBuf, 1, &recvedBytes, &flag, &pOL->m_fromAddr, &pOL->m_addrLen, (LPOVERLAPPED)pOL, 0);
		if (ret == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();

			if (errorCode != WSA_IO_PENDING)
			{
				cout << "Shutdown everything since error happened." << endl;
				// shutdown everything since error happened.
			}
		}
	}
}