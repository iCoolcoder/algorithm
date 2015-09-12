#ifndef __RTP_UPLOAD_H__  
#define __RTP_UPLOAD_H__   

#include "util/DXThread.h"
#include <string>

class RTPUpload : public CDXThread {
    RTPUpload(std::string ip, std::string tcp_port, std::string udp_port);
    ~RTPUpload();
    int Run();
private:
    std::string _ip;
    std::string _tcp_port;
    std::string _udp_port;
};

#endif
