#include "stdafx.h"
#include "Queue.h"


Queue::Queue(void)
{
	head = tail = NULL;
	len = 0;
}


Queue::~Queue(void)
{
	PQElement ele;
	if(head)
	{
		ele = head;
		while(ele)
		{
			delete ele;
			ele = head->next;
		}
	}
}

void Queue::Push(Type data)
{
	PQElement ele = new QElement;
	ele->data = data;
	ele->next = head;
	ele->pre = NULL;
	if(head)
		head->pre = ele;
	else
		tail = ele;
	head = ele;
	len++;
}

int Queue::Pop(Type& data)
{
	if(tail)
	{
		data = tail->data;
		PQElement ele = tail;
		tail = tail->pre;
		if(NULL == tail)
			head = NULL;
		else
			tail->next = NULL;
		delete ele;
		len--;
		return 1;
	}

	return -1;
}

bool Queue::IsEmpty()
{
	if(len > 0)
		return false;

	return true;
}
