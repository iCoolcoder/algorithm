#ifndef _H264_H_
#define _H264_H_

#include <stdint.h>
#include "util/port.h"

namespace avcodec
{
    typedef struct
    {
        int i_ref_idc;  /* nal_priority_e */
        int i_type;     /* nal_unit_type_e */
        int b_long_startcode;
        int i_first_mb; /* If this NAL is a slice, the index of the first MB in the slice. */
        int i_last_mb;  /* If this NAL is a slice, the index of the last MB in the slice. */

        /* Size of payload (including any padding) in bytes. */
        int     i_payload;
        /* If param->b_annexb is set, Annex-B bytestream with startcode.
        * Otherwise, startcode is replaced with a 4-byte size.
        * This size is the size used in mp4/similar muxing; it is equal to i_payload-4 */
        uint8_t *p_payload;

        /* Size of padding in bytes. */
        int i_padding;
    } x264_nal_t;

    DLLEXPORT uint8_t* create_h264_payload(x264_nal_t* x264_nal_list, uint32_t nal_num, uint32_t& payload_len);

    uint8_t const *ff_avc_find_startcode(uint8_t  const *p, uint8_t const *end);
}

#endif /* _H264_H_ */
