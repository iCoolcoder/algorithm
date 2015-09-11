#ifndef __RPT_INFO_H_
#define __RPT_INFO_H_

#include <stdint.h>
#include "avformat/sdp.h"
#include <list>
#include "util/port.h"

class MediaInfo
{
public:
    int32_t clock_rate;
    int32_t bit_rate;
    uint16_t port;
    uint32_t ssrc;
    avformat::RTPAVType payload_type;
    avformat::RTPTransport transport;

    virtual avformat::rtp_media_info* generate_rtp_media_info();
    avformat::RTPMediaType get_media_type();
};

class H264MediaInfo : public MediaInfo
{
public:
    virtual avformat::rtp_media_info* generate_rtp_media_info();

    uint8_t* sps;
    uint32_t sps_len;
    uint8_t* pps;
    uint32_t pps_len;
};

class AACMediaInfo : public MediaInfo
{
public:
    uint8_t audioObjectType;
    uint8_t channels;

    virtual avformat::rtp_media_info* generate_rtp_media_info();
};

class SessionDescription
{
public:
    int start_time;
    int end_time;
    std::string receiver_ip;
    std::string stream_id;
    std::list<MediaInfo*> media_list;

    avformat::SdpInfo* generate_sdp_session_level();
    void parse_sdp_info(avformat::SdpInfo* sdpinfo);
};

class NetworkInfo
{
public:
    uint32_t max_nack_bandwidth_limit;
    uint32_t nack_interval;
    //receiver
    uint32_t max_nacklst_size;
    uint32_t max_packet_age;
    uint32_t sr_rr_interval;
    uint32_t rtt_detect_interval;
    uint32_t session_timeout;

    int32_t max_buffer_duration_ms;
    uint16_t rtp_port;
};

#endif /* __RPT_INFO_H_ */
