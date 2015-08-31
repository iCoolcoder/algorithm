/*
* Author: gaosiyu@youku.com, hechao@youku.com
*/
#include <list>
#include "rtp_trans.h"
#include "rtp_trans_manager.h"
#ifdef LIVESTREAMSDK_EXPORTS
#include "util/log.h"
#endif


void RTPTrans::on_handle_rtp(avformat::RTP_FIXED_HEADER *pkt, uint32_t pkt_len)
{
    if (pkt)
    {
        if (_sessions.find(pkt->get_ssrc()) == _sessions.end()) {
            _sessions[pkt->get_ssrc()] = new RTPSession(this, _mode == SENDER_TRANS_MODE ? SENDER_SESSION_MODE : RECEIVER_SESSION_MODE, _config);

        }

        RTPSession* session = _sessions[pkt->get_ssrc()];
        if (session->is_live())
        {
            session->on_handle_rtp(pkt, pkt_len);
        }
    }
}
void RTPTrans::on_handle_rtcp(avformat::RTCPPacketInformation *rtcpPacketInformation)
{
    if (rtcpPacketInformation)
    {

        if (_sessions.find(rtcpPacketInformation->remoteSSRC) == _sessions.end()) {
            _sessions[rtcpPacketInformation->remoteSSRC] = new RTPSession(this, _mode == SENDER_TRANS_MODE ? SENDER_SESSION_MODE : RECEIVER_SESSION_MODE, _config);
        }

        RTPSession* session = _sessions[rtcpPacketInformation->remoteSSRC];
        if (session->is_live())
        {
            session->on_handle_rtcp(rtcpPacketInformation);
        }
    }
}
void RTPTrans::on_timer()
{
    std::map<uint32_t, RTPSession *>::iterator it;
    std::list<RTPSession *> del_sessions;
    for (it = _sessions.begin(); it != _sessions.end(); it++)
    {
        RTPSession *session = it->second;
        session->on_timer();
        if (!session->is_live())
        {
            del_sessions.insert(del_sessions.end(), session);
        }
    }
    if (!del_sessions.empty())
    {
        std::list<RTPSession *>::iterator session_it;
        for (session_it = del_sessions.begin(); session_it != del_sessions.end(); session_it++)
        {
            RTPSession *session = *(session_it);
            close_session(session);
        }
    }
}

int RTPTrans::send_rtp(uint32_t ssrc, uint16_t seq)
{
    _manager->send_rtp(ssrc, seq);
    return 0;
}
int RTPTrans::send_rtcp(uint32_t ssrc, const avformat::RtcpPacket *pkt)
{
    _manager->send_rtcp(ssrc, pkt);
    return 0;
}

void RTPTrans::close_session(RTPSession *session) {
    /*_sessions.erase(session->get_ssrc());
    delete session;*/
    std::map<uint32_t, RTPSession *>::iterator it = _sessions.find(session->get_ssrc());
    if (it != _sessions.end())
    {
        _sessions.erase(it);
        delete session;
    }

    if (_sessions.empty())
    {
        //_manager->_close_trans_cb(_name);
        DBG("no session remain trans will close streamid %s", _sid.unparse().c_str());
    }
}


void RTPTrans::init_trans_config() {
    _config = new RTPTransConfig;
    _config->max_nack_bandwidth_limit = MAX_NACK_BITRATE;
    _config->max_packet_age = NACK_PKT_MAX_AGE;


    _config->max_nacklst_size = NACK_LIST_MAX_SIZE;
    _config->sr_rr_interval = SR_RR_DELAY;
    _config->rtt_detect_interval = RTT_DELAY;
    _config->session_timeout = RTP_SESSION_TIMEOUT;
    _config->nack_interval = NACK_PROCESS_DELAY;
}

void RTPTrans::destroy() {
    std::map<uint32_t, RTPSession *>::iterator it;
    for (it = _sessions.begin(); it != _sessions.end(); it++)
    {
        RTPSession *session = it->second;
        session->destroy();
    }
    _sessions.clear();
}
