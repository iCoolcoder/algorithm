
#ifndef __RTP_TRANS_MANAGER_
#define __RTP_TRANS_MANAGER_

#include <stdint.h>
#include <deque>
#include <string>
#include <map>

#include "rtp_types.h"
#include "streamid.h"
#include "rtp_trans.h"
#include "avformat/sdp.h"
#include "avformat/rtp.h"
#include "avformat/rtcp.h"
#include "DXLock.h"

const int MAX_RTP_LEN = (10 * 1024);

struct RTPItem
{
    char payload[MAX_RTP_LEN];
    int len;
    uint32_t ssrc;
    uint16_t seq;
};

struct RTPRTCPSendQueueSlot
{
    char payload[MAX_RTP_LEN];
    int len;
};

class RTPStreamCache {
public:
    RTPStreamCache() :
        _max_size(3000),
        _cur_size(0)
    {

    }
    int put_rtp(const avformat::RTP_FIXED_HEADER *rtp, uint16_t len)
    {
        RTPItem item;
        memcpy(item.payload, rtp, len);
        item.len = len;
        item.seq = rtp->get_seq();
        item.ssrc = rtp->get_ssrc();
        if (_cur_size > _max_size)
        {
            _rtp_queue.pop_front();

        }
        _rtp_queue.push_back(item);
        _cur_size++;
        return 0;
    }

    int get_rtp_by_seq(uint32_t seq, char * buf, uint16_t len)
    {
        std::deque<RTPItem>::iterator it;
        for (it = _rtp_queue.begin(); it != _rtp_queue.end(); it++)
        {
            if (it->seq == seq)
            {
                if (len < it->len)
                    return -1;
                memcpy(buf, it->payload, it->len);
                return it->len;
            }
        }
        return -1;
    }
private:
    std::deque<RTPItem> _rtp_queue;
    int _max_size;
    int _cur_size;
};

class RTPTransManager
{
    friend class RTPTrans;
    public:
        RTPTransManager(std::string trans_name, StreamId_Ext sid);
        ~RTPTransManager();

    public:
        int open_receiver();
        int close_receiver();
        int open_sender();
        int close_sender();
        int put_rtp(const void *rtp, uint16_t len);
        int put_rtcp(const void *rtcp, uint32_t len);
        void on_timer();
        void send_rtp(uint32_t ssrc, uint16_t seq);
        void send_rtcp(uint32_t ssrc, const avformat::RtcpPacket *rtcp);
        void send_request(const char *buf, uint32_t len);
        int get_one_rtp_rtcp(char *buf, uint32_t len);

    private:
        std::string _name;
        StreamId_Ext _sid;
        RTPTrans *_rtp_trans;
        CMutex _mutex;
        uint32_t _audio_ssrc;
        RTPStreamCache _audio_stream_cache;
        uint32_t _video_ssrc;
        RTPStreamCache _video_stream_cache;
        std::deque<RTPRTCPSendQueueSlot> _send_queue;
};

#endif

