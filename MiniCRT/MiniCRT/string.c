#include "minicrt.h"

char* mini_itoa(int n, char* str, int radix)
{
	char digit[] = "0123456789ABCDEFGHIGKLMNOPQRSTUVWXYZ";
	char* p = str;
	char* head = str;

	if(!p || radix < 2 || radix > 36)
		return NULL;

	if(radix != 10 || n < 0)
		return p;

	if(n == 0)
	{
		*p++ = '0';
		*p = 0;
		return p;
	}

	if(radix == 10 && n < 0)
	{
		*p++ = '-';
		n = -n;
	}

	while(n)
	{
		*p++ = digit[n % radix];
		n /= radix;
	}

	*p = 0;
	for(--p; head < p; ++head, --p)
	{
		char temp = *head;
		*head = *p;
		*p = temp;
	}
	return str;
}

int mini_strcmp(const char* src, const char* dest)
{
	int ret = 0;
	unsigned char* p1 = (unsigned char*)src;
	unsigned char* p2 = (unsigned char*)dest;

	while(!(ret = *p1 - *p2) && *p2)
		++p1, ++p2;

	if(ret < 0)
		ret = -1;
	else if(ret > 0)
		ret = 1;
	return(ret);
}

char* mini_strcpy(char* dest, const char* src)
{
	char* ret = dest;
	while(*src)
	{
		*dest++ = *src++;
	}
	*dest = '\0';

	return ret;
}

unsigned strlen(const char* str)
{
	int cnt = 0;
	if(!str)
		return 0;

	for(; *str != '\0'; ++str)
	{
		++cnt;
	}
	return cnt;
}