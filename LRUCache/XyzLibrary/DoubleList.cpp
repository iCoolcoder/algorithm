#include "stdafx.h"
#include "DoubleList.h"

void DestroyList(Node *head)
{
	if (!head)
	{
		Node *next;
		Node *pre = next;
		while(pre != NULL)
		{
			next = pre->Next;
			delete pre;
			pre = next;
		}
	}
}

DoubleList::DoubleList(void)
{
	head = tail = NULL;
	len = 0;
}


DoubleList::~DoubleList(void)
{
	DestroyList(head);
}

int DoubleList::PushFront(Type data)
{
	Node *ele = new Node;
	ele->Data = data;
	if (NULL == head)
		head = tail = ele;

	head->Pre = ele;
	ele->Next = head;
	ele->Pre = NULL;
	head = ele;
	len++;
	return 1;
}

int DoubleList::PushBack(Type data)
{
	Node *ele = new Node;
	ele->Data = data;
	if (NULL == head)
		head = tail = ele;
	
	tail->Next = ele;
	ele->Pre = tail;
	ele->Next = NULL;
	tail = ele;
	len++;
	return 1;
}

int DoubleList::DeleteFront()
{
	Node *ele = head;
	if (len == 1)
	{
		head = tail = NULL;
	}
	
	head = head->Next;
	head->Pre = NULL;
	delete ele;
	len--;
	return 1;
}

int DoubleList::DeleteBack()
{
	Node *ele = tail;
	if (len == 1)
	{
		head = tail = NULL;
	}
	
	tail = tail->Pre;
	tail->Next = NULL;
	delete ele;
	len--;
	return 1;
}

