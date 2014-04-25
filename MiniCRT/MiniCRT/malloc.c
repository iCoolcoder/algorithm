#include "minicrt.h"

enum HEAP_FLAG
{
	HEAP_BLOCK_FREE = 0xABABABAB, // magic number of free block
	HEAP_BLOCK_USED = 0xCDCDCDCD, // magic number of used block
};
typedef struct _heap_header
{
	enum HEAP_FLAG type;

	unsigned size;
	struct _heap_header* next;
	struct _heap_header* prev;
} heap_header;

#define ADDR_ADD(a, o) ((char*)(a) + o)
#define HEADER_SIZE (sizeof(heap_header))

static heap_header* list_head = NULL;

void mini_free(void* ptr)
{
	heap_header* header = (heap_header*)ADDR_ADD(ptr, -HEADER_SIZE);

	if(header->type != HEAP_BLOCK_USED)
		return;

	header->type = HEAP_BLOCK_FREE;

	if(header->prev != NULL && header->prev->type == HEAP_BLOCK_FREE)
	{
		// merge
		header->prev->next = header->next;
		if(header->next != NULL)
			header->next->prev = header->prev;
		header->prev->size += header->size;

		header = header->prev;
	}

	if(header->next != NULL && header->next->type == HEAP_BLOCK_FREE)
	{
		// merge
		header->size += header->next->size;
		header->next = header->next->next;
	}
}

void* mini_malloc(unsigned size)
{
	heap_header* header;

	if(size == 0)
		return NULL;

	header = list_head;
	while(header != 0)
	{
		if(header->type == HEAP_BLOCK_USED)
		{
			header = header->next;
			continue;
		}

		if(header->size > size + HEADER_SIZE && header->size <= size + HEADER_SIZE * 2)
		{
			header->type = HEAP_BLOCK_USED;
		}

		if(header->size > size + HEADER_SIZE * 2)
		{
			// split
			heap_header* next = (heap_header*)ADDR_ADD(header, size + HEADER_SIZE);
			next->prev = header;
			next->next = header->next;
			next->type = HEAP_BLOCK_FREE;
			next->size = header->size - (size + HEADER_SIZE);
			header->next = next;
			header->size = size + HEADER_SIZE;
			header->type = HEAP_BLOCK_USED;
			return ADDR_ADD(header, HEADER_SIZE);
		}
		header = header->next;
	}

	return NULL;
}

#ifdef WIN32
#include <Windows.h>
#endif

int mini_crt_heap_init()
{
	void* base = NULL;
	heap_header* header = NULL;
	// 32 MB heap size
	unsigned heap_size = 1024 * 1024 * 32;

#ifdef WIN32
	base = VirtualAlloc(0, heap_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if(base == NULL)
		return 0;
#else
	// TODO: linux part
#endif
	header = (heap_header*)base;

	header->size = heap_size;
	header->type = HEAP_BLOCK_FREE;
	header->next = NULL;
	header->prev = NULL;

	list_head = header;
	return 1;
}