#pragma once

#include <stdint.h>
#ifndef _WIN32
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#endif

namespace avformat
{

    enum RTPTransport
    {
        RTP_AVP = 1,
        RTP_AVPF = 2,
    };

    enum RTPAVType
    {
        RTP_AV_NULL = 0,
        RTP_AV_MP3 = 14,
        RTP_AV_H264 = 96,
        RTP_AV_AAC = 97,
        RTP_AV_ALL = 255,
    };

    enum RTPMediaType
    {
        RTP_MEDIA_NULL = 0,
        RTP_MEDIA_AUDIO = 1,
        RTP_MEDIA_VIDEO = 2,
        RTP_MEDIA_BOTH = 3,
    };
    /******************************************************************
    RTP_FIXED_HEADER
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |V=2|P|X|  CC   |M|     PT      |       sequence number         |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                           timestamp                           |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |           synchronization source (SSRC) identifier            |
    +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
    |            contributing source (CSRC) identifiers             |
    |                             ....                              |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    ******************************************************************/
    typedef struct
    {
        /* byte 0 */
        uint8_t csrc_len : 4; /* CC expect 0 */
        uint8_t extension : 1;/* X  expect 1, see RTP_OP below */
        uint8_t padding : 1;  /* P  expect 0 */
        uint8_t version : 2;  /* V  expect 2 */
        /* byte 1 */
        uint8_t payload : 7; /* PT  RTP_PAYLOAD_RTSP */
        uint8_t marker : 1;  /* M   expect 1 */
        /* byte 2,3 */
        uint16_t seq_no;   /*sequence number*/
        /* byte 4-7 */
        uint32_t timestamp;
        /* byte 8-11 */
        uint32_t ssrc; /* stream number is used here. */
        uint8_t data[0];

        uint32_t get_ssrc() const
        {
            return ntohl(ssrc);
        }

        uint32_t set_ssrc(uint32_t input_ssrc)
        {
            ssrc = htonl(input_ssrc);
            return input_ssrc;
        }

        uint16_t get_seq() const
        {
            return ntohs(seq_no);
        }

        uint16_t set_seq(uint16_t seq)
        {
            seq_no = htons(seq_no);
            return seq;
        }

        uint32_t get_rtp_timestamp()
        {
            return ntohl(timestamp);
        }

        uint32_t set_rtp_timestamp(uint32_t rtp_timestamp)
        {
            timestamp = htonl(rtp_timestamp);
            return rtp_timestamp;
        }

        uint32_t get_ms_timestamp(uint32_t sample_rate)
        {
            uint32_t rtp_timestamp = ntohl(timestamp);
            return ((uint64_t)rtp_timestamp) * 1000 / sample_rate;
        }

        uint32_t set_ms_timestamp(uint32_t ms_timestamp, uint32_t sample_rate)
        {
            uint32_t rtp_timestamp = ((uint64_t)ms_timestamp)* sample_rate / 1000;
            timestamp = htonl(rtp_timestamp);
            return rtp_timestamp;
        }
    } RTP_FIXED_HEADER;/*12 bytes*/

    /******************************************************************
    NALU_HEADER
    +---------------+
    |0|1|2|3|4|5|6|7|
    +-+-+-+-+-+-+-+-+
    |F|NRI|  Type   |
    +---------------+
    ******************************************************************/
    typedef struct {
        //byte 0  
        unsigned char TYPE : 5;
        unsigned char NRI : 2;
        unsigned char F : 1;
    } NALU_HEADER; /* 1 byte */


    /******************************************************************
    FU_INDICATOR
    +---------------+
    |0|1|2|3|4|5|6|7|
    +-+-+-+-+-+-+-+-+
    |F|NRI|  Type   |
    +---------------+
    ******************************************************************/
    typedef struct {
        //byte 0  
        unsigned char TYPE : 5;
        unsigned char NRI : 2;
        unsigned char F : 1;
    } FU_INDICATOR; /*1 byte */


    /******************************************************************
    FU_HEADER
    +---------------+
    |0|1|2|3|4|5|6|7|
    +-+-+-+-+-+-+-+-+
    |S|E|R|  Type   |
    +---------------+
    ******************************************************************/
    typedef struct {
        //byte 0  
        unsigned char TYPE : 5;
        unsigned char R : 1;
        unsigned char E : 1;
        unsigned char S : 1;
    } FU_HEADER; /* 1 byte */


    typedef struct
    {
        //    int startcodeprefix_len;      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)  
        uint32_t len;                 //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)  
        uint32_t max_size;            //! Nal Unit Buffer size  
        int forbidden_bit;            //! should be always FALSE  
        int nal_reference_idc;        //! NALU_PRIORITY_xxxx  
        int nal_unit_type;            //! NALU_TYPE_xxxx      
        uint8_t *buf;                    //! contains the first byte followed by the EBSP
        //    unsigned short lost_packets;  //! true, if packet loss is detected  
    } NALU_t;

    struct LATM_HEADER
    {
        char au_count[2];
        char first_au_size[2];
    };

}
