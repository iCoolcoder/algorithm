#pragma once
#include "SessionManager.h"
#ifndef __GLOBALVARIABLE_H__
#define __GLOBALVARIABLE_H__

class GlobalVariable
{
public:
	static SessionManager* sessionMgr;
	static bool initialized;
};

#endif