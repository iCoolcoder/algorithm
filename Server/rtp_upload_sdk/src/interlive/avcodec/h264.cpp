#include "h264.h"
#include <stdio.h>
#include <string.h>

namespace avcodec
{
    uint8_t* create_h264_payload(x264_nal_t* x264_nal_list, uint32_t nal_num, uint32_t& payload_len)
    {
        if (x264_nal_list == NULL)
        {
            payload_len = 0;
            return NULL;
        }

        uint32_t offset = 0;
        for (int i = 0; i < nal_num; i++)
        {
            x264_nal_t& x264_nal = x264_nal_list[i];
            offset += x264_nal.i_payload;
        }

        payload_len = offset;
        offset = 0;

        uint8_t* payload = new uint8_t[payload_len];

        for (int i = 0; i < nal_num; i++)
        {
            x264_nal_t& x264_nal = x264_nal_list[i];
            memcpy(payload + offset, x264_nal.p_payload, x264_nal.i_payload);
            offset += x264_nal.i_payload;
        }

        return payload;
    }

    // port from ffmpeg avc.c
    // I did not know the details, I hope it is right.
    static const uint8_t *ff_avc_find_startcode_internal(const uint8_t *p, const uint8_t *end)
    {
        const uint8_t *a = p + 4 - ((intptr_t)p & 3);

        for (end -= 3; p < a && p < end; p++) {
            if (p[0] == 0 && p[1] == 0 && p[2] == 1)
                return p;
        }

        for (end -= 3; p < end; p += 4) {
            uint32_t x = *(const uint32_t*)p;
            //      if ((x - 0x01000100) & (~x) & 0x80008000) // little endian
            //      if ((x - 0x00010001) & (~x) & 0x00800080) // big endian
            if ((x - 0x01010101) & (~x) & 0x80808080) { // generic
                if (p[1] == 0) {
                    if (p[0] == 0 && p[2] == 1)
                        return p;
                    if (p[2] == 0 && p[3] == 1)
                        return p + 1;
                }
                if (p[3] == 0) {
                    if (p[2] == 0 && p[4] == 1)
                        return p + 2;
                    if (p[4] == 0 && p[5] == 1)
                        return p + 3;
                }
            }
        }

        for (end += 3; p < end; p++) {
            if (p[0] == 0 && p[1] == 0 && p[2] == 1)
                return p;
        }

        return end + 3;
    }

    const uint8_t *ff_avc_find_startcode(const uint8_t *p, const uint8_t *end)
    {
        const uint8_t *out = ff_avc_find_startcode_internal(p, end);
        if (p < out && out < end && !out[-1]) out--;
        return out;
    }
}
