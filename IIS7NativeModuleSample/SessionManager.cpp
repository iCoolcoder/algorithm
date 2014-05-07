#include "SessionManager.h"
#include "precomp.h"

SessionManager::SessionManager()
{

}


SessionManager::~SessionManager()
{
	_map.erase(_map.begin(), _map.end());
}

void UInt32ToString(__int32 i, bool hex, char* pBuffer, int bufferLength)
{
	if (hex)
	{
		pBuffer[0] = '0';
		pBuffer[1] = 'x';
		sprintf_s(pBuffer + 2, bufferLength - 2, "%I32x", i);
	}
	else
		sprintf_s(pBuffer, bufferLength, "%I32u", i);
}

Session* SessionManager::CreateSession(string sessionId, string clientIP, string clientPort)
{
	Session* session = GlobalVariable::sessionMgr->FindSession(sessionId);
	
	if (!session)
	{
		string sessionId;
		GUID guid;
		CoCreateGuid(&guid);
		char buffer[56];
		int len = 56;
		UInt32ToString(guid.Data1, false, buffer, len);
		sessionId += buffer;
		sessionId += '-';
		UInt32ToString(guid.Data2, false, buffer, len);
		sessionId += buffer;
		sessionId += '-';
		UInt32ToString(guid.Data3, false, buffer, len);
		sessionId += buffer;
		sessionId += '-';
		for (int i = 0; i < 8; i++)
			sessionId += guid.Data4[i];

		session = new Session(sessionId, clientIP, clientPort);
	}
	
	return session;
}

Session* SessionManager::FindSession(string sessionId)
{
	map<string, Session*>::iterator it = _map.find(sessionId);
	if (it != _map.end())
		return it->second;
	else
		return NULL;
}
