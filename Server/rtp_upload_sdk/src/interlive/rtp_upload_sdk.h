#ifndef __RTP_DOWNLOAD_SDK_H__  
#define __RTP_DOWNLOAD_SDK_H__  

#include <stdint.h>
#include <string>
#include "util/port.h"

#if (defined __cplusplus)
extern "C" {
#endif

    DLLEXPORT int __stdcall rtp_upload_sdk_init(std::string ip, std::string tcp_port, std::string udp_port);

    DLLEXPORT int __stdcall put_rtp(const char *buf, uint16_t len);

    DLLEXPORT int __stdcall rtp_upload_sdk_destroy();

#if (defined __cplusplus)
}
#endif

#endif
