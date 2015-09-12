#include "rtp_trans_manager.h"
#include "common/proto.h"
#include "common/proto_rtp_rtcp.h"
#include "util/DXLock.h"
#include "util/buffer.h"
#include <string>

#include "util/log.h"

using namespace std;
using namespace avformat;

RTPTransManager::RTPTransManager(std::string trans_name, StreamId_Ext sid) :
_name(trans_name),
_sid(sid),
_audio_ssrc(0),
_video_ssrc(0),
_rtp_trans(NULL)
{
}

RTPTransManager::~RTPTransManager()
{
    if (NULL != _rtp_trans)
    {
        delete _rtp_trans;
        _rtp_trans = NULL;
    }
}

int RTPTransManager::open_receiver()
{
    CMAutoLock lock(_mutex);
    _rtp_trans = new RTPTrans(_name, _sid, this, RECEIVER_TRANS_MODE);
    return 0;
}

int RTPTransManager::close_receiver()
{
    CMAutoLock lock(_mutex);
    _rtp_trans->destroy();
    return 0;
}

int RTPTransManager::open_sender()
{
    CMAutoLock lock(_mutex);
    _rtp_trans = new RTPTrans(_name, _sid, this, SENDER_TRANS_MODE);
    return 0;
}

int RTPTransManager::close_sender()
{
    CMAutoLock lock(_mutex);
    _rtp_trans->destroy();
    return 0;
}

int RTPTransManager::put_rtp(const void *rtp, uint16_t len)
{
    CMAutoLock lock(_mutex);
    _rtp_trans->on_handle_rtp((avformat::RTP_FIXED_HEADER *)rtp, len);

    avformat::RTP_FIXED_HEADER * rtp_header = (avformat::RTP_FIXED_HEADER *)rtp;

    RTPAVType type = (RTPAVType)rtp_header->payload;
    switch (type)
    {
    case RTP_AV_MP3:
    case RTP_AV_AAC:
        _audio_ssrc = rtp_header->get_ssrc();
        _audio_stream_cache.put_rtp((avformat::RTP_FIXED_HEADER *)rtp, len);
        send_rtp(_audio_ssrc, rtp_header->get_seq());
        break;
    case RTP_AV_H264:
        _video_ssrc = rtp_header->get_ssrc();
        _video_stream_cache.put_rtp((avformat::RTP_FIXED_HEADER *)rtp, len);
        send_rtp(_video_ssrc, rtp_header->get_seq());
        break;
    case RTP_AV_ALL:
    case RTP_AV_NULL:
    default:
        break;
    }

    return 0;
}

int RTPTransManager::put_rtcp(const void *rtcp_data, uint32_t len)
{
    CMAutoLock lock(_mutex);
    avformat::RTCP rtcp(avformat::Clock::GetRealTimeClock());
    avformat::RTCPPacketInformation info;
    rtcp.parse_rtcp_packet((const uint8_t*)rtcp_data, len, info);
    _rtp_trans->on_handle_rtcp(&info);
    return 0;
}

void RTPTransManager::on_timer()
{
    CMAutoLock lock(_mutex);
    _rtp_trans->on_timer();
}

void RTPTransManager::send_rtp(uint32_t ssrc, uint16_t seq)
{
    CMAutoLock lock(_mutex);
    RTPRTCPSendQueueSlot slot;
    char buf[10 * 1024];
    int len = -1;
    if (ssrc == _audio_ssrc)
    {
        len = _audio_stream_cache.get_rtp_by_seq(seq, buf,
            (uint16_t)sizeof(buf));
    }
    else if (ssrc == _video_ssrc)
    {
        len = _video_stream_cache.get_rtp_by_seq(seq, buf,
            (uint16_t)sizeof(buf));
    }
    if (len > 0) {
        buffer * b = buffer_create_max(65536, 65536);
        encode_rtp_u2r_packet((uint8_t *)buf, (uint32_t)len, b);
        
        slot.len = buffer_data_len(b);
        memcpy(slot.payload, buffer_data_ptr(b), slot.len);
        _send_queue.push_back(slot);
        buffer_free(b);
    }
}

void RTPTransManager::send_rtcp(uint32_t ssrc, const RtcpPacket *rtcp)
{
    CMAutoLock lock(_mutex);
    char buf[10 * 1024];
    uint32_t len = -1;
    rtcp->Build((uint8_t *)buf, (size_t *)&len, 10240);
    if (len > 0)
    {
        buffer * b = buffer_create_max(65536, 65536);
        encode_rtcp_u2r_packet((uint8_t *)buf, (uint32_t)len, b);
        RTPRTCPSendQueueSlot slot;
        slot.len = buffer_data_len(b);
        memcpy(slot.payload, buffer_data_ptr(b), slot.len);
        _send_queue.push_back(slot);
        buffer_free(b);
    }
}

void RTPTransManager::send_request(const char *buf, uint32_t len)
{
    CMAutoLock lock(_mutex);
    if (len > 0)
    {
        RTPRTCPSendQueueSlot slot;
        slot.len = len;
        memcpy(slot.payload, buf, len);
        _send_queue.push_back(slot);
    }
}

int RTPTransManager::get_one_rtp_rtcp(char *buf, uint32_t len)
{
    CMAutoLock lock(_mutex);
    if (0 != _send_queue.size())
    {
        int rtp_len = _send_queue.front().len;
        if (len < rtp_len)
            return -1;
        memcpy(buf, _send_queue.front().payload, rtp_len);
        _send_queue.pop_front();
        return rtp_len;
    }

    return -1;
}
