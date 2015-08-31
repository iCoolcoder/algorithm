#include "memory.h"

#include <stdlib.h>
#include <stdio.h>

#ifndef _WIN32
#include <execinfo.h>
#endif

#ifndef CHECK_MEM_MALLOC
void *
mmalloc(size_t size)
{
	void* ptr = malloc(size);
	return ptr;
}

void
mfree(void *ptr)
{
	return free(ptr);
}

void *
mcalloc(size_t nmemb, size_t size)
{
	void* ptr = calloc(nmemb, size);
	return ptr;
}

void *
mrealloc(void *ptr, size_t size)
{
	ptr = realloc(ptr, size);
	return ptr;
}

#else
void
print_callstack(void)
{
	int j, nptrs;
#define SIZE 100
	void *buffer[100];
	char **strings;

	nptrs = backtrace(buffer, SIZE);
	printf("backtrace() returned %d addresses\n", nptrs);

	/* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
	would produce similar output to the following: */

	strings = backtrace_symbols(buffer, nptrs);
	if (strings == NULL) {
		perror("backtrace_symbols");
		exit(EXIT_FAILURE);
	}

	for (j = 0; j < nptrs; j++)
		printf("%s\n", strings[j]);

	free(strings);
}

static int alloced_mem = 0;


void *
mmalloc(size_t size)
{
	print_callstack();
    void* ptr = malloc(size);
	
	uint* ptr_i = (uint*)ptr;
	uint  mem_size = *(ptr_i - 2);

	alloced_mem += mem_size;

	printf("malloc size: %d\n", mem_size);
	printf("alloced_mem: %d\n", alloced_mem);
	printf("ptr: %x\n", ptr);

	printf("\n");

	return ptr;
}

void
mfree(void *ptr)
{
	printf("free\n");

	uint* ptr_i = (uint*)ptr;
	uint  mem_size = *(ptr_i - 2);

	alloced_mem -= mem_size;

	printf("free size: %d\n", mem_size);
	printf("alloced_mem: %d\n", alloced_mem);

	printf("%x\n", ptr);

	printf("\n");

    return free(ptr);
}

void *
mcalloc(size_t nmemb, size_t size)
{
	printf("calloc\n");

	void* ptr = calloc(nmemb, size);

	uint* ptr_i = (uint*)ptr;
	uint  mem_size = *(ptr_i - 2);

	alloced_mem += mem_size;

	printf("calloc size: %d\n", mem_size);
	printf("alloced_mem: %d\n", alloced_mem);
	printf("ptr: %x\n", ptr);

	printf("\n");

	return ptr;
}

void *
mrealloc(void *ptr, size_t size)
{
	printf("realloc\n");

	uint* ptr_i = (uint*)ptr;
	uint  mem_size = *(ptr_i - 2);

	alloced_mem -= mem_size;

	printf("free size: %d\n", mem_size);
	printf("alloced_mem: %d\n", alloced_mem);

	printf("%x\n", ptr);

	ptr = realloc(ptr, size);

	ptr_i = (uint*)ptr;
	mem_size = *(ptr_i - 2);

	alloced_mem += mem_size;

	printf("mrealloc size: %d\n", mem_size);
	printf("alloced_mem: %d\n", alloced_mem);
	printf("ptr: %x\n", ptr);

	return ptr;
}
#endif
