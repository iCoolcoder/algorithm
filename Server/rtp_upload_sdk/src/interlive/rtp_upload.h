#ifndef __RTP_UPLOAD_H__  
#define __RTP_UPLOAD_H__   

#include "streamid.h"
#include "worker.h"
#include "rtp_upload_sdk.h"
#include "util/DXThread.h"
#include "rtp_trans/rtp_trans_manager.h"
#include <string>

class RTPUpload : public CDXThread {
public:
    RTPUpload(std::string ip, 
              std::string tcp_port, 
              std::string udp_port,
              SessionDescription_EXT &sd);
    ~RTPUpload();

    int Init();
    int SendSessionDescription(SessionDescription_EXT &sd);
    int SendStreamStartRequest();
    int SendRTP(const char *buf, uint16_t len);
    int Destroy();
    int Run();
private:
    int SendSDP(string sdp, string sid);

private:
    std::string         _receiver_ip;
    std::string         _receiver_tcp_port;
    std::string         _receiver_udp_port;
    StreamId_Ext        _rtp_stream_id;
    Sender              *_rtp_udp_sender;
    Receiver            *_rtp_udp_receiver;
    RTPTransTimer       *_rtp_trans_timer;
    RTPTransManager     *_rtp_trans_mgr;
};

#endif
