#include "stdafx.h"
#include "Watcher.h"

#define _SECOND 10000000


VOID APIENTRY TimerAPCRoutine(LPVOID pvArgToCompletionRoutine, DWORD dwTimerLowValue, DWORD dwTimerHighValue)
{
	// To do.
	FILE *stream;
	errno_t err;

	if((err = fopen_s(&stream, "c:\\data2.txt", "a+" )) != 0 )
		printf( "The file 'data2' was not opened\n" );
	else
		printf( "The file 'data2' was opened\n" );

	fprintf_s(stream, "%s\n", "hello");
	// Close stream if it is not NULL 
	if(stream)
	{
		if (fclose( stream ))
		{
			printf( "The file 'crt_fopen_s.c' was not closed\n" );
		}
	}
}

Watcher::Watcher(void)
{
	hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
}


Watcher::~Watcher(void)
{
	if(!hTimer)
		CloseHandle(hTimer);
}

BOOL Watcher::Start()
{
	if (hTimer != NULL)
	{
		__int64         qwDueTime;
		LARGE_INTEGER   liDueTime;
		MYDATA          MyData;
		MyData.dwValue = 0;
		BOOL bSuccess;
		__try 
		{
			// Create an integer that will be used to signal the timer 
			// 5 seconds from now.
			qwDueTime = -5 * _SECOND;

			// Copy the relative time into a LARGE_INTEGER.
			liDueTime.LowPart  = (DWORD) (qwDueTime & 0xFFFFFFFF);
			liDueTime.HighPart = (LONG)  (qwDueTime >> 32);

			bSuccess = SetWaitableTimer(
				hTimer,           // Handle to the timer object
				&liDueTime,       // When timer will become signaled
				5000,             // Periodic timer interval of 5 seconds
				TimerAPCRoutine,     // Completion routine
				&MyData,          // Argument to the completion routine
				FALSE );          // Do not restore a suspended system

			if ( bSuccess ) 
			{
				for (; MyData.dwValue < 1000; MyData.dwValue += 100) 
				{
					SleepEx(
						INFINITE,     // Wait forever
						TRUE );       // Put thread in an alertable state
				}
			} 
			else 
			{
				printf("SetWaitableTimer failed with error %d\n", GetLastError());
			}

		} 
		__finally 
		{
			CloseHandle( hTimer );
		}
		return TRUE;
	} 
	else 
	{
		printf("CreateWaitableTimer failed with error %d\n", GetLastError());
		return FALSE;
	}
	
}
