#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>
#include <stddef.h>
#include <time.h>
#ifndef _WIN32
#include <sys/resource.h>
#endif

#include "common.h"

#if (defined __cplusplus && !defined _WIN32)
extern "C"
{
#endif
typedef enum
{
    HTTP_START = 0,
    HTTP_200 = 0,
    HTTP_400,
    HTTP_403,
    HTTP_404,
    HTTP_405,
    HTTP_408,
    HTTP_414,
    HTTP_421,
    HTTP_500,
    HTTP_503,
    HTTP_509,
    HTTP_END,
} http_code;

#define util_min(x, y) ({							\
			typeof(x) min1__ = (x);					\
			typeof(y) min2__ = (y);					\
			(void) (&min1__ == &min2__);			\
			min1__ < min2__ ? min1__ : min2__; })

#define util_max(x, y) ({							\
			typeof(x) max1__ = (x);					\
			typeof(y) max2__ = (y);					\
			(void) (&max1__ == &max2__);			\
			max1__ > max2__ ? max1__ : max2__; })

#ifndef SAFE_RELEASE
#define SAFE_DELETE( x ) \
if (NULL != x) \
{ \
    delete x; \
    x = NULL; \
    }
#endif

static inline int
util_ipfilter_private(uint32_t ip_h)
{
    return (ip_h >= 0x0A000000 && ip_h <= 0x0AFFFFFF) ||
        (ip_h >= 0xAC100000 && ip_h <= 0xAC1FFFFF) ||
        (ip_h >= 0xC0A80000 && ip_h <= 0xC0A8FFFF);
}

static inline int
util_ipfilter_public(uint32_t ip_h)
{
    if((ip_h >= 0x01000000 && ip_h < 0x0A000000)
       || (ip_h > 0x0AFFFFFF && ip_h <= 0x7EFFFFFF))
        return TRUE;
    if((ip_h >= 0x80000000 && ip_h < 0xA9FE0000)
       || (ip_h > 0xA9FEFFFF && ip_h < 0xAC100000) || (ip_h > 0xAC1FFFFF
                                                       && ip_h <= 0xBFFFFFFF))
        return TRUE;
    if((ip_h >= 0xC0000000 && ip_h < 0xC0A80000)
       || (ip_h > 0xC0A8FFFF && ip_h <= 0xDFFFFFFF))
        return TRUE;
    return FALSE;
}

static inline bool 
is_big_endian()
{
    unsigned short test = 0x1122;
    if (*((unsigned char*)&test) == 0x11)
        return TRUE;
    else
        return FALSE;
}

/* networt to host seq of long long */
unsigned long long util_ntohll(unsigned long long val);

/* host to networt seq of long long */
unsigned long long util_htonll(unsigned long long val);

#if (defined __cplusplus && !defined _WIN32)
}
#endif
#endif
