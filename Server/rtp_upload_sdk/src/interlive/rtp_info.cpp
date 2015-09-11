#include "rtp_info.h"
#include "avcodec/aac.h"

using namespace avformat;
using namespace avcodec;
using namespace std;

SdpInfo* SessionDescription::generate_sdp_session_level()
{
    SdpInfo* sdp = new SdpInfo();

    sdp_session_level* sdp_header = sdp->get_sdp_header();

    sdp_header->sdp_version = 0;
    sdp_header->id = 0;
    sdp_header->version = 0;
    sdp_header->start_time = start_time;
    sdp_header->end_time = end_time;
    sdp_header->ttl = 0;
    sdp_header->user = "-";
    sdp_header->src_addr = "127.0.0.1";
    sdp_header->src_type = "IP4";
    sdp_header->dst_addr = receiver_ip.c_str();
    sdp_header->dst_type = "IP4";
    sdp_header->name = stream_id.c_str();

    list<MediaInfo*>::iterator it = media_list.begin();

    for (; it != media_list.end(); it++)
    {
        rtp_media_info* info = (*it)->generate_rtp_media_info();
        sdp->add_media(info);
    }

    return sdp;
}

rtp_media_info* MediaInfo::generate_rtp_media_info()
{
    rtp_media_info* info = new rtp_media_info();

    info->rate = clock_rate;
    info->payload_type = payload_type;
    info->rtp_transport = transport;
    info->extra_data_len = 0;
    info->dest_port = port;

    return info;
}

avformat::RTPMediaType MediaInfo::get_media_type()
{
    switch (payload_type)
    {
    case avformat::RTP_AV_NULL:
        return avformat::RTP_MEDIA_NULL;
        break;
    case avformat::RTP_AV_MP3:
        return avformat::RTP_MEDIA_AUDIO;
        break;
    case avformat::RTP_AV_H264:
        return avformat::RTP_MEDIA_VIDEO;
        break;
    case avformat::RTP_AV_AAC:
        return avformat::RTP_MEDIA_AUDIO;
        break;
    case avformat::RTP_AV_ALL:
        return avformat::RTP_MEDIA_BOTH;
        break;
    default:
        break;
    }
    return RTP_MEDIA_NULL;
}

rtp_media_info* H264MediaInfo::generate_rtp_media_info()
{
    rtp_media_info* info = MediaInfo::generate_rtp_media_info();

    return info;
}

rtp_media_info* AACMediaInfo::generate_rtp_media_info()
{
    AACConfig aac_config;

    uint8_t samplingFrequencyIndex = aac_get_samplingFrequencyIndex(clock_rate);

    aac_config.set_audioObjectType(audioObjectType);
    aac_config.set_samplingFrequencyIndex(samplingFrequencyIndex);
    aac_config.set_channelConfig(channels);

    rtp_media_info* info = MediaInfo::generate_rtp_media_info();

    info->channels = channels;
    info->extra_data = new uint8_t[2];
    memcpy(info->extra_data, aac_config.audioSpecificConfig, 2);
    info->extra_data_len = 2;

    return info;
}

void SessionDescription::parse_sdp_info(avformat::SdpInfo* sdpinfo) {

    stream_id = sdpinfo->get_sdp_header()->name;
    receiver_ip = sdpinfo->get_sdp_header()->dst_addr;
    start_time = 0;
    end_time = 0;

    vector<rtp_media_info*>::iterator media_it;
    vector<rtp_media_info*> media_infoes = sdpinfo->get_media_infos();
    for (media_it = media_infoes.begin(); media_it != media_infoes.end(); media_it++)
    {
        rtp_media_info* info = *(media_it);
        if (info->payload_type == RTP_AV_H264)
        {
            H264MediaInfo* h264_media_info = new H264MediaInfo();
            h264_media_info->clock_rate = 90000;
            h264_media_info->payload_type = RTP_AV_H264;
            h264_media_info->transport = RTP_AVP;
            h264_media_info->port = info->dest_port;
            media_list.push_back(h264_media_info);
        }
        else if (info->payload_type == RTP_AV_AAC)
        {
            AACMediaInfo* aac_media_info = new AACMediaInfo();
            aac_media_info->clock_rate = info->rate;
            aac_media_info->audioObjectType = info->h264_profile_level_id;
            aac_media_info->channels = info->channels;
            aac_media_info->payload_type = RTP_AV_AAC;
            aac_media_info->transport = RTP_AVP;
            aac_media_info->port = info->dest_port;
            media_list.push_back(aac_media_info);
        }
    }
}
