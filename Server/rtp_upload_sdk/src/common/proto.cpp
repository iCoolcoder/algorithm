#define _BSD_SOURCE
#include "proto.h"
#include <stdint.h>
#include <assert.h>
#include "util/buffer.h"
#include "util/util.h"
#include "util/log.h"

#ifdef _WIN32
#define assert(x) (x)
#endif

#define MAKE_BUF_USABLE(obuf, total_sz) \
{\
    int ret = 0; \
if (buffer_capacity(obuf) < total_sz)\
{\
    ret = buffer_expand_capacity(obuf, total_sz); \
if (0 != ret)\
    return ret; \
}\
}

static int encode_make_buf_usable(buffer* buf, uint32_t total_sz)
{
    MAKE_BUF_USABLE(buf, total_sz);

    return total_sz;
}

void
encode_header(buffer * obuf, uint16_t cmd, uint32_t size)
{
    uint16_t ncmd = htons(cmd);
    uint32_t nsize = htonl(size);

    buffer_append_ptr(obuf, &MAGIC_1, sizeof(uint8_t));
    buffer_append_ptr(obuf, &VER_1, sizeof(uint8_t));
    buffer_append_ptr(obuf, &ncmd, sizeof(uint16_t));
    buffer_append_ptr(obuf, &nsize, sizeof(uint32_t));
}

void encode_header_rtp(char* obuf, size_t obuf_size, const proto_header& header)
{
    assert(obuf != NULL && obuf_size >= sizeof(proto_header));

    proto_header* ph_obuf = (proto_header*)obuf;
    ph_obuf->magic = header.magic;
    ph_obuf->version = header.version;
    ph_obuf->cmd = htons(header.cmd);
    ph_obuf->size = htonl(header.size);
}

void encode_header_uint8(uint8_t* obuf, uint32_t& len, uint16_t cmd, uint32_t size)
{
    uint16_t ncmd = htons(cmd);
    uint32_t nsize = htonl(size);

    obuf[0] = MAGIC_1;
    obuf[1] = VER_1;
    memcpy(&obuf[2], &ncmd, 2);
    memcpy(&obuf[4], &size, 4);

    len = sizeof(proto_header);
}

int decode_header_uint8(const uint8_t * ibuf, uint32_t len, proto_header * h)
{
    if (len < sizeof(proto_header))
        return -1;
    proto_header *ih = (proto_header *)ibuf;

    switch (ih->version)
    {
    case VER_1:
        break;

    case VER_2:
        if (ih->magic != MAGIC_2)
            return -1;
        break;

    case VER_3:
        if (ih->magic != MAGIC_3)
            return -1;
        break;

    default:
        return -1;
    }

    if (h != NULL)
    {
        h->magic = ih->magic;
        h->version = ih->version;
        h->cmd = ntohs(ih->cmd);
        h->size = ntohl(ih->size);
    }
    else
    {
        return -1;
    }

    return 0;
}

int decode_header(const buffer * ibuf, proto_header * h)
{
    if (buffer_data_len(ibuf) < sizeof(proto_header))
        return -1;
    proto_header *ih = (proto_header *)buffer_data_ptr(ibuf);

    switch (ih->version)
    {
    case VER_1:
        break;

    case VER_2:
        if (ih->magic != MAGIC_2)
            return -1;
        break;

    case VER_3:
        if (ih->magic != MAGIC_3)
            return -1;
        break;

    default:
        return -1;
    }

    if (h != NULL)
    {
        h->magic = ih->magic;
        h->version = ih->version;
        h->cmd = ntohs(ih->cmd);
        h->size = ntohl(ih->size);
    }
    else
    {
        return -1;
    }

	return 0;
}

int decode_header_rtp(const char * ibuf, size_t ibuf_size, proto_header& h)
{
    assert(ibuf != NULL && ibuf_size >= sizeof(proto_header));

    proto_header* ph_ibuf = (proto_header*)ibuf;
    h.magic = ph_ibuf->magic;
    h.version = ph_ibuf->version;
    h.cmd = ntohs(ph_ibuf->cmd);
    h.size = ntohl(ph_ibuf->size);
    return 0;
}
