#pragma once

typedef int T;

class CircularQueue
{
public:
	CircularQueue(size_t size);
	~CircularQueue(void);
	bool EnQueue(const T& data);
	bool DeQueue(T& data);
	bool IsFull() const;
	bool IsEmpty() const;
private:
	T* _list;
	size_t _count;
	size_t _size;
	size_t _startIndex;
	size_t _endIndex;
};

