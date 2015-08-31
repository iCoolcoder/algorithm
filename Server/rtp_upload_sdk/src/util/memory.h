#ifndef MEMORY_H_
#define MEMORY_H_

#ifndef _WIN32
#include <unistd.h>
#endif

#if (defined __cplusplus && !defined _WIN32)
extern "C"
{
#endif

void *mmalloc(size_t size);
void mfree(void *ptr);
void *mcalloc(size_t nmemb, size_t size);
void *mrealloc(void *ptr, size_t size);

#if (defined __cplusplus && !defined _WIN32)
}
#endif
#endif
