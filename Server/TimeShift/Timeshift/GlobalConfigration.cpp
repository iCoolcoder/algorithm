#include "stdafx.h"
#include "GlobalConfigration.h"

list<string> GlobalConfigration::channels;
lru_cache_using_std_s<string, string>* GlobalConfigration::sharedCache = NULL;
boost::unordered_map<string, circular_buffer_s<string>*> GlobalConfigration::channel_buffers;
map<string, FILE*> GlobalConfigration::channel_files;
const size_t GlobalConfigration::sharedCacheSize = 10000;
MemPool::CMemoryPool *GlobalConfigration::pBufMgr = NULL;


