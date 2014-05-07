#include "precomp.h"
#include "session.h"


Session::Session(string sessionId, string clientIP, string clientPort)
{
	_sessionId = sessionId;
}


Session::~Session()
{

}

string Session::ReadNext()
{
	return "page content";
}
