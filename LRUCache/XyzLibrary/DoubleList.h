#pragma once

template<class T>
struct Node
{
	T Data;
	Node *Pre;
	Node *Next;
	size_t Len;
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
	Node<T>* PushFront(T& data)
	{
		Node<T> *ele = new Node<T>;
		ele->Data = data;
		if (NULL == head)
		{
			ele->Pre = NULL;
			ele->Next = NULL;
			head = tail = ele;
			len++;
			return ele;
		}

		head->Pre = ele;
		ele->Next = head;
		ele->Pre = NULL;
		head = ele;
		len++;
		return ele;
	}
	int MoveToFront(Node<T>* ele)
	{ 
		if (head == NULL || head->Next == NULL || ele == head)
			return 1;
		
		ele->Pre->Next = ele->Next;
		if (ele->Next != NULL)
			ele->Next->Pre = ele->Pre;
		else
			tail = ele->Pre;

		ele->Pre = NULL;
		head->Pre = ele;
		head = ele;

		return 1;
	}
	void DeleteBack()
	{
		if (head == NULL)
			return;

		tail->Pre->Next = NULL;
		Node<T>* ele = tail;
		tail = tail->Pre;
		if (NULL == tail)
			head = NULL;
		delete ele;
		len--;
	}
	Node<T>* GetTail()
	{
		return tail;
	}
private:
	void DestroyList(Node<T> *head)
	{
		if (!head)
		{
			Node<T> *next = head;
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

