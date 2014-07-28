#pragma once

#include <WinSock2.h>
#include <MSWSock.h>
#include <exception>

using namespace std;

struct SOCKETOVERLAPPED
{
	OVERLAPPED	m_ol;
	SOCKET		m_serverSocket;
	SOCKET		m_clientSocket;
	LPVOID		m_pBuf;
	int			m_opType;
	sockaddr    m_fromAddr;
	int         m_addrLen;

};

class CGRSMsSockFun
{
public:
	CGRSMsSockFun(SOCKET skTemp = INVALID_SOCKET)
	{
		if( INVALID_SOCKET != skTemp )
		{
			LoadAllFun(skTemp);
		}
	}

	virtual ~CGRSMsSockFun(void)
	{
	}

protected:
	BOOL LoadWSAFun(SOCKET& skTemp, GUID& funGuid, void*& pFun)
	{
		DWORD dwBytes = 0;
		BOOL bRet = TRUE;
		pFun = NULL;
		BOOL bCreateSocket = FALSE;
		try
		{
			if(INVALID_SOCKET == skTemp)
			{
				skTemp = ::WSASocket(AF_INET,SOCK_STREAM, IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);
				bCreateSocket = (skTemp != INVALID_SOCKET);
			}

			if(INVALID_SOCKET == skTemp)
			{
				char buffer[100];
				//throw exception(_itoa_s(WSAGetLastError(), buffer, 10));
			}

			if(SOCKET_ERROR == ::WSAIoctl(skTemp,
					SIO_GET_EXTENSION_FUNCTION_POINTER,
					&funGuid,sizeof(funGuid),
					&pFun,sizeof(pFun),&dwBytes,NULL,
					NULL))
			{
					pFun = NULL;
					char buffer[100];
					//throw exception(_itoa_s(WSAGetLastError(), buffer, 10));
			}
		}
		catch(exception& e)
		{
			if(bCreateSocket)
			{
			::closesocket(skTemp);
			}
		}

		return NULL != pFun;
	}

protected:
	LPFN_ACCEPTEX m_pfnAcceptEx;
	LPFN_CONNECTEX m_pfnConnectEx;
	LPFN_DISCONNECTEX m_pfnDisconnectEx;
	LPFN_GETACCEPTEXSOCKADDRS m_pfnGetAcceptExSockaddrs;
	LPFN_TRANSMITFILE m_pfnTransmitfile;
	LPFN_TRANSMITPACKETS m_pfnTransmitPackets;
	LPFN_WSARECVMSG m_pfnWSARecvMsg;

protected:
	BOOL LoadAcceptExFun(SOCKET &skTemp)
	{
		GUID GuidAcceptEx = WSAID_ACCEPTEX;
		return LoadWSAFun(skTemp,GuidAcceptEx,(void*&)m_pfnAcceptEx);
	}

	BOOL LoadConnectExFun(SOCKET &skTemp)
	{
		GUID GuidAcceptEx = WSAID_CONNECTEX;
		return LoadWSAFun(skTemp,GuidAcceptEx,(void*&)m_pfnConnectEx);
	}

	BOOL LoadDisconnectExFun(SOCKET&skTemp)
	{
		GUID GuidDisconnectEx = WSAID_DISCONNECTEX;
		return LoadWSAFun(skTemp,GuidDisconnectEx,(void*&)m_pfnDisconnectEx);
	}

	BOOL LoadGetAcceptExSockaddrsFun(SOCKET &skTemp)
	{
		GUID GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
		return LoadWSAFun(skTemp,GuidGetAcceptExSockaddrs,(void*&)m_pfnGetAcceptExSockaddrs);
	}

	BOOL LoadTransmitFileFun(SOCKET&skTemp)
	{
		GUID GuidTransmitFile = WSAID_TRANSMITFILE;
		return LoadWSAFun(skTemp,GuidTransmitFile,(void*&)m_pfnTransmitfile);
	}

	BOOL LoadTransmitPacketsFun(SOCKET&skTemp)
	{
		GUID GuidTransmitPackets = WSAID_TRANSMITPACKETS;
		return LoadWSAFun(skTemp,GuidTransmitPackets,(void*&)m_pfnTransmitPackets);
	}

	BOOL LoadWSARecvMsgFun(SOCKET&skTemp)
	{
		GUID GuidTransmitPackets = WSAID_TRANSMITPACKETS;
		return LoadWSAFun(skTemp,GuidTransmitPackets,(void*&)m_pfnWSARecvMsg);
	}

public:
	BOOL LoadAllFun(SOCKET skTemp)
	{
		//注意这个地方的调用顺序，是根据服务器的需要，并结合了表达式副作用
		//而特意安排的调用顺序
		return (LoadAcceptExFun(skTemp) &&
					LoadGetAcceptExSockaddrsFun(skTemp) &&
					LoadTransmitFileFun(skTemp) &&
					LoadTransmitPacketsFun(skTemp) &&
					LoadDisconnectExFun(skTemp) &&
					LoadConnectExFun(skTemp) &&
					LoadWSARecvMsgFun(skTemp));
	}

public:
	BOOL AcceptEx(
			  SOCKET sListenSocket,
			  SOCKET sAcceptSocket,
			  PVOID lpOutputBuffer,
			  DWORD dwReceiveDataLength,
			  DWORD dwLocalAddressLength,
			  DWORD dwRemoteAddressLength,
			  LPDWORD lpdwBytesReceived,
			  LPOVERLAPPED lpOverlapped
			  )
	{
		return m_pfnAcceptEx(sListenSocket,
				sAcceptSocket,lpOutputBuffer,
				dwReceiveDataLength,dwLocalAddressLength,
				dwRemoteAddressLength,lpdwBytesReceived,
				lpOverlapped);
	}

	BOOL ConnectEx(
			  SOCKET s,const struct sockaddr FAR *name,
			  int namelen,PVOID lpSendBuffer,
			  DWORD dwSendDataLength,LPDWORD lpdwBytesSent,
			  LPOVERLAPPED lpOverlapped
			  )
	{
		return m_pfnConnectEx(
				s,name,namelen,lpSendBuffer,
				dwSendDataLength,lpdwBytesSent,
				lpOverlapped
				);
	}

	BOOL DisconnectEx(
			  SOCKET s,LPOVERLAPPED lpOverlapped,
			  DWORD  dwFlags,DWORD  dwReserved
			  )
	{
		return m_pfnDisconnectEx(s,
				lpOverlapped,dwFlags,dwReserved);
	}

	VOID GetAcceptExSockaddrs (
			  PVOID lpOutputBuffer,
			  DWORD dwReceiveDataLength,
			  DWORD dwLocalAddressLength,
			  DWORD dwRemoteAddressLength,
			  sockaddr **LocalSockaddr,
			  LPINT LocalSockaddrLength,
			  sockaddr **RemoteSockaddr,
			  LPINT RemoteSockaddrLength
			  )
	{
		return m_pfnGetAcceptExSockaddrs(
			lpOutputBuffer,dwReceiveDataLength,
			dwLocalAddressLength,dwRemoteAddressLength,
			LocalSockaddr,LocalSockaddrLength,
			RemoteSockaddr,RemoteSockaddrLength);
	}

	BOOL TransmitFile(
		 SOCKET hSocket,HANDLE hFile,
		 DWORD nNumberOfBytesToWrite,
		 DWORD nNumberOfBytesPerSend,
		 LPOVERLAPPED lpOverlapped,
		 LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers,
		 DWORD dwReserved
		 )
	{
		return m_pfnTransmitfile(
				hSocket,hFile,nNumberOfBytesToWrite,
				nNumberOfBytesPerSend,lpOverlapped,
				lpTransmitBuffers,dwReserved
				);
	}

	BOOL TransmitPackets(
		 SOCKET hSocket,                            
		 LPTRANSMIT_PACKETS_ELEMENT lpPacketArray,                               
		 DWORD nElementCount,DWORD nSendSize,               
		 LPOVERLAPPED lpOverlapped,DWORD dwFlags                              
		 )

	{

		return m_pfnTransmitPackets(
				hSocket,lpPacketArray,nElementCount,
				nSendSize,lpOverlapped,dwFlags);

	}

	INT WSARecvMsg(
			  SOCKET s,LPWSAMSG lpMsg,
			  LPDWORD lpdwNumberOfBytesRecvd,
			  LPWSAOVERLAPPED lpOverlapped,
			  LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
	{
		return m_pfnWSARecvMsg(
				s,lpMsg,lpdwNumberOfBytesRecvd,
				lpOverlapped,lpCompletionRoutine);
	}
};

