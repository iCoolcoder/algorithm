#pragma once

#ifndef __SESSION_H__
#define __SESSION_H__
class Session
{
public:
	Session(string sessionId, string clientIP, string clientPort);
	~Session();
	string ReadNext();
private:
	string _sessionId;
	string _pageData;
};

#endif