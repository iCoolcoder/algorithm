#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "util.h"
#include <stdint.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

#ifndef _WIN32
#include <unistd.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sched.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#endif

#define UTIL_ERR(...) fprintf(stderr, __VA_ARGS__);\
	fprintf(stderr, "\n")

static char *g_http_code[HTTP_END][2] = {
	{ "200", "OK" },
	{ "400", "Bad Request" },
	{ "403", "Forbidden" },
	{ "404", "Not Found" },
	{ "405", "Method Not Allowed" },
	{ "408", "Request Timeout" },
	{ "414", "Request-URI Too Long" },
	{ "421", "There are too many connections from your internet address" },
	{ "500", "Internal Server Error" },
	{ "503", "Service Unavailable" },
	{ "509", "Bandwidth Limit Exceeded" },
};

unsigned long long
util_ntohll(unsigned long long val)
{
    if (!is_big_endian()) {
        return (((unsigned long long) htonl((int)((val << 32) >> 32))) <<
            32) | (unsigned int)htonl((int)(val >> 32));
    }
    else {
        return val;
    }
}

unsigned long long
util_htonll(unsigned long long val)
{
    if (!is_big_endian()) {
        return (((unsigned long long) htonl((int)((val << 32) >> 32))) <<
            32) | (unsigned int)htonl((int)(val >> 32));
    }
    else {
        return val;
    }
}

