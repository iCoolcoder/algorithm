#include "precomp.h"

//custom http headers
const char SESSION_ID[] = "HEADER_sessionid";
const char ENCRYPTED_URL[] = "HEADER_url";

//  Implementation of the OnAcquireRequestState method
REQUEST_NOTIFICATION_STATUS
CMyHttpModule::OnAcquireRequestState(
    IN IHttpContext *                       pHttpContext,
    IN OUT IHttpEventProvider *             pProvider
)
{
    HRESULT                         hr = S_OK;

	// TODO: implement the AcquireRequestState module functionality

Finished:

    if ( FAILED( hr )  )
    {
        return RQ_NOTIFICATION_FINISH_REQUEST;
    }
    else
    {
        return RQ_NOTIFICATION_CONTINUE;
    }
}

// TODO: implement other desired event handler methods below
REQUEST_NOTIFICATION_STATUS
CMyHttpModule::OnResolveRequestCache(
IN IHttpContext *                       pHttpContext,
IN OUT IHttpEventProvider *             pProvider
)
{
	pContext = pHttpContext;
	pRequest = pHttpContext->GetRequest();
	pResponse = pHttpContext->GetResponse();
	pRawRequest = pRequest->GetRawHttpRequest();

	return OnOpen();
}

// Start a connection and stream
REQUEST_NOTIFICATION_STATUS CMyHttpModule::OnOpen()
{
	FILE* fp;
	fp = fopen("C:\\log.txt", "w");
	fprintf(fp, "%s\n", "begin");
	//Get sock Info
	string currentNicAddr = GetServerVariable("LOCAL_ADDR");
	string localPort = GetServerVariable("SERVER_PORT");
	string clientIP = GetServerVariable("REMOTE_ADDR");
	string clientPort = GetServerVariable("REMOTE_PORT");

	//Get the raw parameters from client request
	url = GetServerVariable(ENCRYPTED_URL);
	sessionId = GetServerVariable(SESSION_ID);

	fprintf(fp, "%s\n", "mid");

	Session* session = GlobalVariable::sessionMgr->CreateSession(sessionId, clientIP, clientPort);

	string pageData = session->ReadNext();
	//Prepare memory
	HTTP_DATA_CHUNK aChunk;
	BOOL bCompletionExpected = TRUE;
	DWORD dwBytesWritten = 0;

	aChunk.DataChunkType = HttpDataChunkFromMemory;
	aChunk.FromMemory.pBuffer = (void*)pageData.c_str();
	aChunk.FromMemory.BufferLength = static_cast<ULONG>(2);

	HRESULT hr = pResponse->WriteEntityChunks(&aChunk, 1, FALSE, TRUE, &dwBytesWritten, &bCompletionExpected);
	fprintf(fp, "%s\n", "completed");
	fclose(fp);

	return RQ_NOTIFICATION_FINISH_REQUEST;
}

string CMyHttpModule::GetServerVariable(PCSTR VarName)
{
	string ServerVar;
	PCWSTR pServerVar;
	DWORD ServerVarLen;
	HRESULT hr;

	hr = pContext->GetServerVariable(VarName, &pServerVar, &ServerVarLen);
	if (SUCCEEDED(hr))
		ServerVar = CW2A(pServerVar);

	return ServerVar;
}