#include "stdafx.h"
#include "CSafeQueue.h"
#include <Windows.h>
#include <stdlib.h>

CSafeQueue::CSafeQueue(int nMaxElements)
	: m_hmtxQ(m_h[0]), m_hsemNumElements(m_h[1])
{
	m_pElements = (PELEMENT)HeapAlloc(GetProcessHeap(), 0, sizeof(ELEMENT) * nMaxElements);
	m_nMaxElements = nMaxElements;
	m_hmtxQ = CreateMutex(NULL, FALSE, NULL);
	m_hsemNumElements = CreateSemaphore(NULL, 0, nMaxElements, NULL);
}

CSafeQueue::~CSafeQueue()
{
	CloseHandle(m_hmtxQ);
	CloseHandle(m_hsemNumElements);
	HeapFree(GetProcessHeap(), 0, m_pElements);
}

BOOL CSafeQueue::Append(PELEMENT pElement, DWORD dwTimeout)
{
	BOOL fOk = FALSE;
	DWORD dw = WaitForSingleObject(m_hmtxQ, dwTimeout);

	if (dw == WAIT_OBJECT_0)
	{
		// This thread has exclusive access to the queue
		// Increment the number of elements in the queue
		LONG lPrevCount;
		fOk = ReleaseSemaphore(m_hsemNumElements, 1, &lPrevCount);
		if (fOk)
		{
			// The queue is not full, append the new element
			m_pElements[lPrevCount] = *pElement;
		}
		else
		{
			// The queue is full, set the error code and return failure
			SetLastError(ERROR_DATABASE_FULL);
		}
	}
	else
	{
		// Timeout, set error code and return failure
		SetLastError(ERROR_TIMEOUT);
	}

	return(fOk);
}

BOOL CSafeQueue::Remove(PELEMENT pElement, DWORD dwTimeout)
{
	// Wait for exclusive access to queue and for queue to have element
	BOOL fOk = (WaitForMultipleObjects(_countof(m_h), m_h, TRUE, dwTimeout) == WAIT_OBJECT_0);

	if (fOk)
	{
		// The queue has an element, pull it from the queue
		*pElement = m_pElements[0];

		// Shift the remaining elements down
		MoveMemory(&m_pElements[0], &m_pElements[1], sizeof(ELEMENT) * (m_nMaxElements - 1));

		// Allow other threads to access the queue
		ReleaseMutex(m_hmtxQ);

	}
	else
	{
		// Timeout
		SetLastError(ERROR_TIMEOUT);
	}

	return(fOk);
}