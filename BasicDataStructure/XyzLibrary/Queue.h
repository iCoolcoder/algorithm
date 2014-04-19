#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the XYZLIBRARY_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// XYZAPI functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef XYZLIBRARY_EXPORTS
#define XYZAPI __declspec(dllexport)
#else
#define XYZAPI __declspec(dllimport)
#endif

typedef int Type;

typedef struct QElement
{
	Type data;
	struct QElement* pre;
	struct QElement* next;
}*PQElement;

class XYZAPI Queue
{
public:
	Queue(void);
	~Queue(void);
	void Push(Type data);
	int Pop(Type& data);
	bool IsEmpty();
private:
	PQElement head;
	PQElement tail;
	size_t len;
};

