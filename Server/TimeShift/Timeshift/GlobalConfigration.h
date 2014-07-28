#pragma once
#include "SafeCircularCache.h"
#include "SafeLRUCache.h"
#include "MemPool\CMemoryPool.h"
#include <map>
#include <string>
#include <boost\unordered_map.hpp>

using namespace std;

class GlobalConfigration
{
public:
	static list<string> channels;
	static lru_cache_using_std_s<string, string>* sharedCache;
	static boost::unordered_map<string, circular_buffer_s<string>*> channel_buffers;
	static map<string, FILE*> channel_files;
	static const size_t sharedCacheSize;
	static MemPool::CMemoryPool *pBufMgr;
};

