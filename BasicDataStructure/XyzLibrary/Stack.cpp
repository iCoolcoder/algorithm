#include "stdafx.h"
#include "Stack.h"

Stack::Stack()
{
	head = NULL;
	len = 0;
}

Stack::~Stack()
{
	if(head)
	{
		PElement ele = head;
		while(ele)
		{
			head = head->next;
			delete ele;
			ele = head;
		}
	}
}

int Stack::Pop(Type& data)
{
	if(head)
	{
		data = head->data;
		PElement ele = head;
		head = head->next;
		delete ele;
		len--;
		return 1;
	}

	return -1;
}

void Stack::Push(Type data)
{
	PElement ele = new struct Element;
	
	ele->data = data;
	ele->next = head;
	head = ele;
	len++;
}

bool Stack::IsEmpty()
{
	if(len > 0)
		return false;

	return true;
}