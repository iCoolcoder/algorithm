#include "rtp_session.h"

#include "util/log.h"

RTPSession::RTPSession(RTPTrans * trans, RTPSessionMode mode, RTPTransConfig *config) {
    _parent = trans;
    _mode = mode;
    _last_process_srts = 0;
    _last_process_rrts = 0;
    _last_process_nackts = 0;
    _last_process_rtt = 0;
    _last_keeplive_ts = 0;
    _last_bitrate_ts = 0;

    _last_rtp_timestamp = 0;
    _total_rtp_sendbytes = 0;
    _total_rtp_packetcount = 0;
    _last_sender_report = 0;
    _frac_nack_sendbytes = 0;
    _frac_rtp_sendbytes = 0;

    _frac_lost_packet_count = 0;
    _total_lost_packet_count = 0;

    _lastseq = 0;
    _send_bitrate = 0;
    _rtt_ms = 0;
    _ssrc = 0;
    _alive = true;
    _m_clock = avformat::Clock::GetRealTimeClock();
    _payload_type = avformat::RTP_AV_NULL;
    this->_config = config;
}
RTPSession::~RTPSession() {
    //_alive = false;
    _nacks.clear();
}
void RTPSession::on_handle_rtp(avformat::RTP_FIXED_HEADER *pkt, uint32_t pkt_len) {
    if (_ssrc == 0)
    {
        _ssrc = pkt->get_ssrc();
    }
    if (_payload_type = avformat::RTP_AV_NULL)
    {
        _payload_type = (avformat::RTPAVType)pkt->payload;
    }
    if (_mode == RECEIVER_SESSION_MODE)
    {

        int seq = pkt->get_seq();

        if (_lastseq == 0)
        {
            _lastseq = seq;
        }

        if (is_newer_seq(seq, _lastseq)) {

            for (uint16_t i = _lastseq + 1; is_newer_seq(seq, i); i++)
            {
                //if (i > lastoutseq)
                //{
                //printf("receiver request retrans %d %d \n", i, seq);
                _nacks.insert(_nacks.end(), i);
                //}
            }
        }
        else {
            //printf("receive retrans packet remove from nacklst %d \n", seq);
            _nacks.erase(seq);
            _total_rtp_packetcount--;
            _total_rtp_sendbytes -= pkt_len;
            _frac_rtp_sendbytes -= pkt_len;
        }



        _lastseq = is_newer_seq(_lastseq, seq) ? _lastseq : seq;

        //check nacklist size is over 250
        while (isNacklistToolarge()) {
            //remove until the first item in the nacklist is keyframe
            //removeUntilKeyFrame()
            _nacks.erase(_nacks.begin());
        }

        //check nackitem is delay 450 packet
        while (isNacklistTooOld()) {
            //remove until the first item in the nacklist is keyframe
            //removeUntilKeyFrame()
            _nacks.erase(_nacks.begin());
        }
    }
    else {
        _last_rtp_timestamp = pkt->timestamp;
    }
    _frac_rtp_sendbytes += pkt_len;
    _total_rtp_sendbytes += pkt_len;
    _total_rtp_packetcount++;
}
void RTPSession::on_handle_rtcp(avformat::RTCPPacketInformation *rtcpPacketInformation) {
    if (_ssrc == 0)
    {
        _ssrc = rtcpPacketInformation->remoteSSRC;
    }

    if (rtcpPacketInformation->rtcpPacketTypeFlags == avformat::kRtcpNack) {
        std::list<uint16_t> nack_items = rtcpPacketInformation->nackSequenceNumbers;
        std::list<uint16_t>::iterator it;
        uint32_t bytes_re_sent = 0;
        for (it = nack_items.begin(); it != nack_items.end(); it++)
        {
            uint16_t seq = *it;
            int ret = _send_rtp(_ssrc, seq);
            if (ret > 0 && _rtt_ms && _config->max_nack_bandwidth_limit)
            {
                bytes_re_sent += ret;
                size_t target_bytes =
                    (static_cast<size_t>(_config->max_nack_bandwidth_limit / 1000) * _rtt_ms) >> 3;
                if (bytes_re_sent > target_bytes) {
                    break;  // Ignore the rest of the packets in the list.
                }
            }
            _frac_nack_sendbytes += ret > 0 ? ret : 0;
        }
    }
    else if (rtcpPacketInformation->rtcpPacketTypeFlags == avformat::kRtcpSr)
    {
        //_last_process_srts = _m_clock->TimeInMilliseconds();
        _last_sender_report = _m_clock->NtpToMs(rtcpPacketInformation->ntp_secs, rtcpPacketInformation->ntp_frac);
        uint32_t old_lost_count = _total_lost_packet_count;
        _total_lost_packet_count = rtcpPacketInformation->sendPacketCount - _total_rtp_packetcount;
        _frac_lost_packet_count = static_cast<uint8_t>((float)(_total_lost_packet_count - old_lost_count) * 1000 / _config->sr_rr_interval);
    }
    else if (rtcpPacketInformation->rtcpPacketTypeFlags == avformat::kRtcpRr)
    {
        avformat::ReportBlockList report_blocks = rtcpPacketInformation->report_blocks;
        uint32_t max_rtt = 0;
        avformat::ReportBlockList::iterator it;
        for (it = report_blocks.begin(); it != report_blocks.end(); it++)
        {
            avformat::RTCPReportBlock report_block = *(it);
            uint32_t delay_rr_ms =
                (((report_block.delaySinceLastSR & 0x0000ffff) * 1000) >> 16) +
                (((report_block.delaySinceLastSR & 0xffff0000) >> 16) * 1000);
            int64_t rtt = _m_clock->CurrentNtpInMilliseconds() - delay_rr_ms - report_block.lastSR;
            max_rtt = rtt > max_rtt ? rtt : max_rtt;
        }
        _rtt_ms = max_rtt;
    }
    else if (rtcpPacketInformation->rtcpPacketTypeFlags == avformat::kRtcpBye)
    {
        _alive = false;
    }

    _last_keeplive_ts = _m_clock->TimeInMilliseconds();
}

void RTPSession::on_timer()
{
    if (_mode == RECEIVER_SESSION_MODE)
    {
        //process nacks
        if (_last_process_nackts == 0 || _m_clock->TimeInMilliseconds() - _last_process_nackts > _config->nack_interval)
        {
            if (!_nacks.empty())
            {
                //check nacklist size is over 250
                while (isNacklistToolarge()) {
                    //remove until the first item in the nacklist is keyframe
                    _nacks.erase(_nacks.begin());
                }
                //sendNackPacket(NackPacket(nacklst))
                avformat::Nack nack;

                int nack_length = _nacks.size();
                uint16_t *nack_array = new uint16_t[nack_length];
                std::set<uint16_t, SequenceNumberLessThan>::iterator it;
                int i = 0;
                for (i = 0, it = _nacks.begin(); it != _nacks.end(); i++, it++)
                {
                    nack_array[i] = *it;
                    //printf("retrans %d %d\n", nack_array[i], nacks.size());
                }
                nack.From(_ssrc);
                nack.WithList(nack_array, nack_length);

                //sendrtcppacket
                if (_ssrc)
                {
                    _send_rtcp(_ssrc, &nack);
                }
                DBG("rtp session send send nack rtcp pkt %d size %d rtt %d ms ", _ssrc, _nacks.size(), _rtt_ms);
            }
            _last_process_nackts = _m_clock->TimeInMilliseconds();
        }
        //process rr
        if (_last_process_rrts == 0 || _m_clock->TimeInMilliseconds() - _last_process_rrts > _config->sr_rr_interval)
        {
            avformat::ReportBlock report_block;
            report_block.To(_ssrc);
            report_block.WithCumulativeLost(_total_lost_packet_count);
            report_block.WithFractionLost(_frac_lost_packet_count);
            report_block.WithLastSr(_last_sender_report);
            // DLSR in unit of 1/65536 seconds
            report_block.WithDelayLastSr((_m_clock->TimeInMilliseconds() - _last_process_srts) >> 16);
            avformat::ReceiverReport rr;
            rr.From(_ssrc);
            rr.WithReportBlock(&report_block);
            avformat::Sdes seds;
            seds.WithCName(_ssrc, RTP_CNAME);
            rr.Append(&seds);
            if (_ssrc)
            {
                _send_rtcp(_ssrc, &rr);
            }
            _last_process_rrts = _m_clock->TimeInMilliseconds();
        }
    }
    //process sr
    else if (_mode == SENDER_SESSION_MODE)
    {
        if (_last_process_srts == 0 || _m_clock->TimeInMilliseconds() - _last_process_srts > _config->sr_rr_interval)
        {
            avformat::SenderReport sr;
            uint32_t ntp_secs, ntp_frac;
            _m_clock->CurrentNtp(ntp_secs, ntp_frac);
            sr.From(_ssrc);
            sr.WithNtpSec(ntp_secs);
            sr.WithNtpFrac(ntp_frac);
            sr.WithRtpTimestamp(_last_rtp_timestamp);
            sr.WithOctetCount(_total_rtp_sendbytes);
            sr.WithPacketCount(_total_rtp_packetcount);

            avformat::Sdes seds;
            seds.WithCName(_ssrc, RTP_CNAME);
            sr.Append(&seds);
            if (_ssrc)
            {
                _send_rtcp(_ssrc, &sr);
            }
            _last_process_srts = _m_clock->TimeInMilliseconds();
        }
        if (_last_bitrate_ts == 0 || _m_clock->TimeInMilliseconds() - _last_bitrate_ts > _config->rtt_detect_interval)
        {
            _send_bitrate = static_cast<uint32_t>((float)((_frac_rtp_sendbytes - _frac_nack_sendbytes) << 3) / (float)(_m_clock->TimeInMilliseconds() - _last_bitrate_ts) * 1000);
            // printf("bitrate: %d %d %d\n", send_bitrate, frac_rtp_sendbytes, frac_nack_sendbytes);
            _frac_rtp_sendbytes = 0;
            _frac_nack_sendbytes = 0;
            _last_bitrate_ts = _m_clock->TimeInMilliseconds();
        }

    }
}

bool RTPSession::is_live()
{
    if (!_alive)
    {
        return false;
    }


    bool b_timout = _last_keeplive_ts == 0 || _m_clock->TimeInMilliseconds() - _last_keeplive_ts < _config->session_timeout;
    if (!b_timout)
    {
        DBG("rtp session timeout %d", _ssrc);
    }
    return b_timout;
}

bool RTPSession::is_newer_seq(uint16_t seq, uint16_t preseq) {
    return (seq != preseq) &&
        static_cast<uint16_t>(seq - preseq) < 0x8000;
}

int RTPSession::isNacklistToolarge() {
    return _config->max_nacklst_size && _nacks.size() > _config->max_nacklst_size;
}

int RTPSession::isNacklistTooOld() {

    return _config->max_packet_age && !_nacks.empty() && _lastseq - *(_nacks.begin()) > _config->max_packet_age;
}

uint32_t RTPSession::get_ssrc() {
    return _ssrc;
}


uint32_t RTPSession::_send_rtcp(uint32_t ssrc, avformat::RtcpPacket *pkt)
{
    int ret = _parent->send_rtcp(ssrc, pkt);
    return ret;
}

uint32_t RTPSession::_send_rtp(uint32_t ssrc, uint16_t seq)
{
    int ret = _parent->send_rtp(ssrc, seq);
    return ret;
}

void RTPSession::destroy() {
    if (_parent)
    {
        avformat::Bye bye;
        bye.From(_ssrc);
        _send_rtcp(_ssrc, &bye);
    }
}

