/*
* Author: gaosiyu@youku.com, hechao@youku.com
*/

#ifndef __RTP_TRANS_
#define __RTP_TRANS_

#include <string>
#include <map>
//#include <hash_map>
#include "avformat/rtp.h"
#include "avformat/rtcp.h"
#include "rtp_session.h"
#include "streamid.h"
#include "rtp_config.h"

enum RTPTransMode
{
    SENDER_TRANS_MODE,
    RECEIVER_TRANS_MODE
};


class RTPTransConfig;
class RTPSession;
class RTPTransManager;
class RTPTrans
{
public:

    RTPTrans(std::string trans_name, StreamId_Ext sid, RTPTransManager *manager, RTPTransMode mode, RTPTransConfig *config = NULL)
        :_name(trans_name), _manager(manager), _sid(sid), _mode(mode), _config(config)
    {
        if (config == NULL)
            init_trans_config();
    }

    //        virtual ~RTPtrans(){}

public:
    virtual void on_handle_rtp(avformat::RTP_FIXED_HEADER *pkt, uint32_t pkt_len);
    virtual void on_handle_rtcp(avformat::RTCPPacketInformation *rtcpPacketInformation);
    virtual void on_timer();
    virtual void destroy();

    std::string get_name() const { return _name; }
    StreamId_Ext get_sid() const { return _sid; }

protected:
    virtual int send_rtp(uint32_t ssrc, uint16_t seq);
    virtual int send_rtcp(uint32_t ssrc, const avformat::RtcpPacket *pkt);
    void close_session(RTPSession *session);

    void init_trans_config();

private:
    friend class RTPSession;
    std::string _name;
    RTPTransManager * _manager;
    StreamId_Ext _sid;
protected:
    std::map<uint32_t, RTPSession *> _sessions;
    RTPTransMode _mode;
    RTPTransConfig *_config;
};

#endif

