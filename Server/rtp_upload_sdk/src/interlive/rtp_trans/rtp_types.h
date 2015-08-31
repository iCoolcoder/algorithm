/*
 * Author: hechao@youku.com 
 *
 */
#ifndef __RTP_TYPE_
#define __RTP_TYPE_

typedef unsigned short rtp_seq_t;
enum rtp_type_t
{
    RTP_TYPE_SDP,
    RTP_TYPE_RTP,
    RTP_TYPE_RTCP
};

#endif

