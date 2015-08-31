#ifndef __MD5_H__
#define __MD5_H__

/* MD5.H - header file for MD5C.C
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */
#include <limits.h>
#include <stddef.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#if (defined __cplusplus && !defined _WIN32)
extern "C"
{
#endif

#define UINT4 unsigned int
#define UINT2 uint16_t
#define POINTER unsigned char *

/* MD5 context. */
typedef struct
{
    UINT4 state[4];             /* state (ABCD) */
    UINT4 count[2];             /* number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[64];   /* input buffer */
} MD5_CTX;

void MD5_Init(MD5_CTX *);
void MD5_Update(MD5_CTX *, const void *, unsigned int);
void MD5_Final(unsigned char[16], MD5_CTX *);
void MD5_Final_str(char md5_str[], size_t sz, MD5_CTX *);

#if (defined __cplusplus && !defined _WIN32)
}
#endif
#endif
