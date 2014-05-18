#pragma once

template<class T>
class U_Ptr
{
	friend class Smart_Ptr;
	U_Ptr(T* ptr): ptr(ptr), refTimes(1);
	~U_Ptr(void)
	{
		delete ptr;
	}
	T* ptr;
	size_t refTimes;
};


template<class T>
class Smart_Ptr
{
public:
	Smart_Ptr(T* realPtr = 0): pointee(new U_Ptr(p));
	Smart_Ptr(const Smart_Ptr &rhs): pointee(rhs.pointee)
	{
		++pointee->refTimes;
	}
	~Smart_Ptr(void)
	{
		if(--pointee->refTimes == 0)
			delete pointee;
	}
	Smart_Ptr& operator=(Smart_Ptr &rhs)
	{
		++rhs.pointee->refTimes;

		if (--pointee->refTimes == 0)
			delete pointee;
		pointee = rhs.pointee;

		return *this;
	}
	T* operator->() const
	{
		return pointee->ptr;
	}
	T& operator*() const
	{
		return *(pointee->ptr);
	}
private:
	U_Ptr<T>* pointee; // object that smart pointer points to
};


