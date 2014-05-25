#pragma once
#include <string>
using namespace std;

typedef string Type;

struct Node
{
	Type Data;
	Node *Pre;
	Node *Next;
};

class DoubleList
{
public:
	DoubleList(void);
	~DoubleList(void);
	int PushFront(Type data);
	int PushBack(Type data);
	int DeleteFront();
	int DeleteBack();
	Node* Find(Type data);
	int Remove(Node* ele);
private:
	Node *head;
	Node *tail;
	size_t len;
};

