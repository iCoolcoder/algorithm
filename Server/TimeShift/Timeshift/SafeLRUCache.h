#ifndef _lru_safe_cache_using_std_
#define _lru_safe_cache_using_std_

#include <cassert>
#include <map>
#include <list>
#include <Windows.h>

template <typename K, typename V>
class lru_cache_using_std_s
{
public:
    typedef K key_type;
    typedef V value_type;

    // save the sequence, the oldest the lastest
    typedef std::list<key_type> key_tracker_type;

    // save Key/Value data
    typedef std::map<
        key_type,
        std::pair<value_type, typename key_tracker_type::iterator>
        > key_to_value_type;

    lru_cache_using_std_s(value_type (*f)(const key_type&), size_t c)
        : fn_(f), capacity_(c)
    {
        assert(capacity_!=0);
		InitializeCriticalSection(&lock_);
    }
	
	~lru_cache_using_std_s()
	{
		DeleteCriticalSection(&lock_);
	}

    // interface
    value_type operator()(const key_type& k)
    {
		EnterCriticalSection(&lock_);
        const typename key_to_value_type::iterator it = key_to_value_.find(k);

        if (it == key_to_value_.end()) {
            const value_type v = fn_(k);
            insert(k, v);
            return v;
        } else {
            // exist already, adjust the order
            key_tracker_.splice(key_tracker_.end(),
                                key_tracker_,
                                (*it).second.second);
            return (*it).second.first;
        }
		LeaveCriticalSection(&lock_);
    }

private:
    // insert new
    void insert(const key_type& k, const value_type& v)
    {
        assert(key_to_value_.find(k)==key_to_value_.end());

        // when cache is full, evict oldest
        if (key_to_value_.size() == capacity_) evict();

        typename key_tracker_type::iterator it
            = key_tracker_.insert(key_tracker_.end(), k);

        key_to_value_.insert(std::make_pair(k,
                std::make_pair(v,it)));
    }

    // evict old
    void evict()
    {
        assert(!key_tracker_.empty());

        // the top is the oldest
        const typename key_to_value_type::iterator it
            = key_to_value_.find(key_tracker_.front());

        assert(it!=key_to_value_.end());

        // remove both in map and list
        key_to_value_.erase(it);
        key_tracker_.pop_front();
    }

	// When cache missing, get value via key. Usually it visit a lower speed resource, such as network or disk
    value_type (*fn_)(const key_type&);

    size_t capacity_;

    key_tracker_type key_tracker_;
    key_to_value_type key_to_value_;
	static CRITICAL_SECTION lock_;
};

template <typename K, typename V>
CRITICAL_SECTION lru_cache_using_std_s<K, V>::lock_;

#endif