#include "stdafx.h"
#include "CircularQueue.h"

CircularQueue::CircularQueue(size_t size)
{
	_list = new T[size];
	_count = 0;
	_size = size;
	_startIndex = 0;
	_endIndex = 0;
}


CircularQueue::~CircularQueue(void)
{
	if(_list)
		delete[] _list;
}

bool CircularQueue::EnQueue(const T& data)
{
	if(IsFull())
		return false;

	_list[_endIndex++] = data;
	_count++;

	if(_endIndex > _size - 1)
		_endIndex = 0;

	return true;
}

bool CircularQueue::DeQueue(T& data)
{
	if(IsEmpty())
		return false;

	data = _list[_startIndex++];
	_count--;

	if(_startIndex > _size - 1)
		_startIndex = 0;

	return true;
}

bool CircularQueue::IsFull() const
{
	if(_count == _size)
		return true;
	else
		return false;
}

bool CircularQueue::IsEmpty() const
{
	if(_count == 0)
		return true;
	else
		return false;
}
