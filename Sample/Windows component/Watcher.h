#pragma once

typedef struct _MYDATA {
   TCHAR *szText;
   DWORD dwValue;
} MYDATA;

class Watcher
{
public:
	Watcher(void);
	~Watcher(void);
	BOOL Start();
private:
	HANDLE hTimer;
};

