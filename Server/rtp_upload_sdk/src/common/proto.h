#ifndef PROTO_H_
#define PROTO_H_

#include <stdint.h>

#include "common/proto_define.h"
#include "util/buffer.h"

const uint8_t VER_1 = 1; // v1 tracker protocol: with original forward_client/server
const uint8_t VER_2 = 2; // v2 tracker protocol: with module_tracker in forward
const uint8_t VER_3 = 3; // v3 tracker protocol: with module_tracker in forward, support long stream id

const uint8_t MAGIC_1 = 0xff;
const uint8_t MAGIC_2 = 0xf0;
const uint8_t MAGIC_3 = 0x0f;

#if (defined __cplusplus && !defined _WIN32)
extern "C" 
{
#endif

void encode_header(buffer * obuf, uint16_t cmd, uint32_t size);
void encode_header_rtp(char* obuf, size_t obuf_size, const proto_header &header);
void encode_header_uint8(uint8_t * obuf, uint32_t& len, uint16_t cmd, uint32_t size);
int decode_header(const buffer * ibuf, proto_header * h);
int decode_header_rtp(const char * ibuf, size_t ibuf_size, proto_header &h);
int decode_header_uint8(const uint8_t * ibuf, uint32_t len, proto_header * h);

#if (defined __cplusplus && !defined _WIN32)
}
#endif
#endif
