#pragma once

template<class T>
struct Node
{
	T Data;
	Node *Pre;
	Node *Next;
};


template<class T>
class DoubleList
{
public:
	DoubleList(void)
	{
		head = tail = NULL;
		len = 0;
	}
	~DoubleList(void)
	{
		DestroyList(head);
	}
	int PushFront(T& data)
	{
		Node<T> *ele = new Node<T>;
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
	int MoveToFront(Node<T>* ele)
	{ 
		if (head == NULL || head->next = NULL || ele = head)
			return 1;
		
		ele->pre->next = ele->next;
		if (ele->next != NULL)
			ele->next->pre = ele->pre;
		else
			tail = ele->pre;

		ele->pre = NULL;
		head->pre = ele;
		head = ele;

		return 1;
	}
private:
	void DestroyList(Node<T> *head)
	{
		if (!head)
		{
			Node<T> *next;
			Node<T> *pre = next;
			while(pre != NULL)
			{
				next = pre->Next;
				delete pre;
				pre = next;
			}
		}
	}
private:
	Node<T> *head;
	Node<T> *tail;
	size_t len;
};

