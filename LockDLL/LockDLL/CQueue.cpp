#include "stdafx.h"
#include "CQueue.h"

int CQueue::GetFreeSlot()
{
	// Look for the first element with a 0 stamp
	for (int current = 0; current < m_nMaxElements; current++)
	{
		if (m_pElements[current].m_nStamp == 0)
			return(current);
	}

	// No free slot found
	return(-1);
}

int CQueue::GetNextSlot(int nThreadNum)
{
	// By default, there is no slot for this thread
	int firstSlot = -1;
	
	// The element can't have a stamp higher than the last added
	int firstStamp = m_nCurrentStamp + 1;

	// Look for the even / odd element that is not free
	for (int current = 0; current < m_nMaxElements; current++)
	{
		// Keep track of the first added (lowest stamp) in the queue
		// --> so that "first in first out" behavior is ensured
		if ((m_pElements[current].m_nStamp != 0) &&  //free element
			((m_pElements[current].m_element.m_nRequestNum % 2) == nThreadNum) && 
			(m_pElements[current].m_nStamp < firstStamp))
		{
			firstStamp = m_pElements[current].m_nStamp;
			firstSlot = current;
		}
	}

	return(firstSlot);
}

void CQueue::AddElement(ELEMENT e)
{
	// Do nothing if the queue is full
	int nFreeSlot = GetFreeSlot();
	if (nFreeSlot == -1)
	{
		return;
	}

	// Copy the content of the element
	m_pElements[nFreeSlot].m_element = e;

	// Mark the element with new stamp
	m_pElements[nFreeSlot].m_nStamp = ++m_nCurrentStamp;
}

BOOL CQueue::GetNewElement(int nThreadNum, ELEMENT &e)
{
	int nNewSlot = GetNextSlot(nThreadNum);
	if (nNewSlot == -1)
		return(FALSE);

	// Copy the content of the element
	e = m_pElements[nNewSlot].m_element;

	// Mark the element as read
	m_pElements[nNewSlot].m_nStamp = 0;

	return(TRUE);
}

BOOL CQueue::IsFull()
{
	//TODO
	return(TRUE);
}

BOOL CQueue::IsEmpty(int nThreadNum)
{
	// TODO
	return(TRUE);
}

CQueue::CQueue(int nMaxElements)
{
	m_pElements = new INNER_ELEMENT[m_nMaxElements];
}

CQueue::~CQueue()
{
	if (!m_pElements)
		delete[] m_pElements;
}