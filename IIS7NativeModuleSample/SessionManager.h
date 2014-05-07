#pragma once
#ifndef __SESSIONMANAGER_H__
#define __SESSIONMANAGER_H__
#include <map>
#include "session.h"
#include "GlobalVariable.h"
#include <stdint.h>
using namespace std;

class SessionManager
{
public:
	SessionManager();
	~SessionManager();
	Session* CreateSession(string sessionId, string clientIP, string clientPort);
	Session* FindSession(string sessionId);
private:
	map<string, Session*> _map;
};

#endif