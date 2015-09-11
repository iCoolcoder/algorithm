#ifndef __RTP_DOWNLOAD_SDK_H__  
#define __RTP_DOWNLOAD_SDK_H__  

#include <stdint.h>
#include <string>
#include <list>
#include "util/port.h"
#include "avformat/rtp.h"

#if (defined __cplusplus)
extern "C" {
#endif

    struct NetworkInfo_EXT
    {
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

    struct MediaInfo_EXT
    {
    public:
        int32_t clock_rate;
        int32_t bit_rate;
        uint16_t port;
        uint32_t ssrc;
        avformat::RTPAVType payload_type;
        avformat::RTPTransport transport;
    };

    struct H264MediaInfo_EXT : public MediaInfo_EXT
    {
        uint8_t* sps;
        uint32_t sps_len;
        uint8_t* pps;
        uint32_t pps_len;
    };

    struct AACMediaInfo_EXT : public MediaInfo_EXT
    {
        uint8_t audioObjectType;
        uint8_t channels;
    };

    struct SessionDescription_EXT
    {
        int start_time;
        int end_time;
        std::string receiver_ip;
        std::string stream_id;
        AACMediaInfo_EXT *aac_media_info;
        H264MediaInfo_EXT *h264_media_info;
    };
    
    DLLEXPORT int rtp_upload_sdk_init(std::string ip, std::string tcp_port, std::string udp_port, SessionDescription_EXT &sd);
    DLLEXPORT int put_session_description(SessionDescription_EXT &sd);
    DLLEXPORT int put_request();
    DLLEXPORT int put_rtp(const char *buf, uint16_t len);
    DLLEXPORT int rtp_upload_sdk_destroy();

#if (defined __cplusplus)
}
#endif

#endif
