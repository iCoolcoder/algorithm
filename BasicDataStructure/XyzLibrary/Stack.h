#pragma once

#ifdef XYZLIBRARY_EXPORTS
#define XYZAPI __declspec(dllexport)
#else
#define XYZAPI __declspec(dllimport)
#endif

typedef int Type;

typedef struct Element
{
	Type data;
	struct Element* next;
}*PElement;

class XYZAPI Stack
{
private:
	PElement head;
	size_t len;
public:
	Stack();
	~Stack();
	void Push(Type data);
	int Pop(Type& data);
	bool IsEmpty();
};

