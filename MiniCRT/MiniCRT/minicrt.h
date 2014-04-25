#ifndef __MINI_CRT_H__
#define __MINI_CRT_H__

#ifdef __cplusplus
extern "C" {
#endif

// malloc
#ifndef NULL
#define NULL (0)
#endif

void mini_free(void* ptr);
void* mini_malloc(unsigned size);
int mini_crt_init_heap();

// string
int mini_strcmp(const char* src, const char* dst);
char* mini_itoa(int n, char* str, int radix);
char* mini_strcpy(char* dest, const char* src);
unsigned mini_strlen(const char* str);

// IO
typedef int FILE;

#define EOF (-1)

int mini_crt_init_io();
FILE* mini_fopen(const char* filename, const char* mode);

#ifdef __cplusplus
}
#endif

#endif // __MINI_CRT_H__
