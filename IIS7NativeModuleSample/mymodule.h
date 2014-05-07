#include <string>
#include <atlstr.h>

using namespace std;

#ifndef __MY_MODULE_H__
#define __MY_MODULE_H__

//  The module implementation.
//  This class is responsible for implementing the 
//  module functionality for each of the server events
//  that it registers for.
class CMyHttpModule : public CHttpModule
{
public:

	//  Implementation of the AcquireRequestState event handler method.
    REQUEST_NOTIFICATION_STATUS
    OnAcquireRequestState(
        IN IHttpContext *                       pHttpContext,
        IN OUT IHttpEventProvider *             pProvider
    );

	//  TODO: override additional event handler methods below.
	REQUEST_NOTIFICATION_STATUS
	OnResolveRequestCache(
		IN IHttpContext *                       pHttpContext,
		IN OUT IHttpEventProvider *             pProvider
		);
private:
	REQUEST_NOTIFICATION_STATUS OnOpen();
	string GetServerVariable(PCSTR VarName);
private:
	IHttpContext*	pContext;
	IHttpRequest*	pRequest;
	IHttpResponse*	pResponse;
	HTTP_REQUEST*	pRawRequest;
	string url;
	string sessionId;
};

#endif
