#ifndef RTP_SESSION_HEADER
#define RTP_SESSION_HEADER
#include <stdint.h>
#include "../avformat/rtp.h"
#include "../avformat/rtcp.h"
#include "rtp_trans.h"
#include "rtp_config.h"

#define NACK_LIST_MAX_SIZE 250
#define NACK_PKT_MAX_AGE 450
#define SR_RR_DELAY 5000
#define NACK_PROCESS_DELAY 10
#define RTT_DELAY 10
#define RTP_SESSION_TIMEOUT 10000
#define RTP_CNAME "laifeng.com"
#define MAX_NACK_BITRATE 1 << 23

typedef std::set<uint16_t> NACK_QUEUE;

class RTPTrans;
struct RTPControlConfig;
enum RTPSessionMode
{
    SENDER_SESSION_MODE,
    RECEIVER_SESSION_MODE
};


class RTPSession
{
    class SequenceNumberLessThan {
    public:
        bool operator() (const uint16_t& sequence_number1,
            const uint16_t& sequence_number2) const {
            return is_newer_seq(sequence_number2, sequence_number1);
        }
    };

    typedef std::set<uint16_t, SequenceNumberLessThan> NACK_QUEUE;

public:
    RTPSession(RTPTrans * trans, RTPSessionMode mode, RTPTransConfig *config);
    ~RTPSession();
    //event
    virtual void on_handle_rtp(avformat::RTP_FIXED_HEADER *pkt, uint32_t pkt_len);
    virtual void on_handle_rtcp(avformat::RTCPPacketInformation *rtcpPacketInformation);
    virtual void on_timer();
    //method
    uint32_t get_ssrc();
    uint32_t get_frac_lost_packet_count() { return _frac_lost_packet_count; }
    uint32_t get_total_lost_packet_count() { return _total_lost_packet_count; }
    uint32_t get_rtt_ms() { return _rtt_ms; }
    uint32_t get_effect_bitrate() { return _send_bitrate; }
    avformat::RTPAVType get_payload_type(){ return _payload_type; }
    void set_payload_type(avformat::RTPAVType type){ _payload_type = type; }
    bool is_live();
    void destroy();

protected:

    static bool is_newer_seq(uint16_t seq, uint16_t preseq);
    int isNacklistToolarge();
    int isNacklistTooOld();

    virtual uint32_t _send_rtcp(uint32_t ssrc, avformat::RtcpPacket *pkt);
    virtual uint32_t _send_rtp(uint32_t ssrc, uint16_t seq);

private:
    //for sender report
    uint32_t _last_rtp_timestamp;
    uint32_t _total_rtp_sendbytes;
    uint32_t _total_rtp_packetcount;

    uint32_t _frac_nack_sendbytes;
    uint32_t _frac_rtp_sendbytes;
    uint32_t _last_rr_recv_byte;

    //for receiver report
    uint32_t _last_sender_report;
    //statistics
    uint8_t  _frac_lost_packet_count;
    uint32_t _total_lost_packet_count;
    uint32_t _rtt_ms;
    uint32_t _send_bitrate;
    //need RFC 5401
    //uint32_t recv_bitrate;

    uint64_t _last_process_srts;
    uint64_t _last_process_rrts;
    uint64_t _last_process_nackts;
    uint64_t _last_process_rtt;
    uint64_t _last_keeplive_ts;
    uint64_t _last_bitrate_ts;
    uint16_t _lastseq;
    //uint32_t since_sr_sendbytes;


    uint32_t _ssrc;
    RTPTrans *_parent;
    NACK_QUEUE _nacks;
    RTPSessionMode _mode;
    //test
    RTPTransConfig *_config;
    bool _alive;
    avformat::RTPAVType _payload_type;
public:
    avformat::Clock* _m_clock;

};
#endif
