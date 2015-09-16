#include "rtp_upload_sdk.h"
#include "rtp_upload.h"
#include "util/util.h"
#include <string>
#include "util/log.h"

using namespace std;

#if (defined __cplusplus)
extern "C" DLLEXPORT int rtp_upload_sdk_init(string ip, string tcp_port, string udp_port, SessionDescription_EXT &sd);
extern "C" DLLEXPORT int put_session_description(SessionDescription_EXT &sd);
extern "C" DLLEXPORT int put_request();
extern "C" DLLEXPORT int put_rtp(const char *buf, uint16_t len);
extern "C" DLLEXPORT int rtp_upload_sdk_destroy();
#endif

static RTPUpload    *p_rtp_upload = NULL;

int rtp_upload_sdk_init(string ip, string tcp_port, string udp_port, SessionDescription_EXT &sd)
{
    LOGER::open();
    fprintf(LOGER::fp, "open\n");
    fflush(LOGER::fp);
    p_rtp_upload = new RTPUpload(ip,
        tcp_port,
        udp_port,
        sd);
    if (NULL == p_rtp_upload)
    {
        return -1;
    }

    if (0 != p_rtp_upload->Init())
    {
        fprintf(LOGER::fp, "error\n");
        fflush(LOGER::fp);
        return -1;
    }
    fprintf(LOGER::fp, "right\n");
    fflush(LOGER::fp);
    return 0;
}

int put_session_description(SessionDescription_EXT &sd)
{
    fprintf(LOGER::fp, "session22\n");
    fflush(LOGER::fp);
    if (0 != p_rtp_upload->SendSessionDescription(sd))
    {
        return -1;
    }
    fprintf(LOGER::fp, "session33\n");
    fflush(LOGER::fp);
    return 0;
}

int put_request()
{
    if (0 != p_rtp_upload->SendStreamStartRequest())
    {
        return -1;
    }
    return 0;
}

int put_rtp(const char *buf, uint16_t len)
{
    if (0 != p_rtp_upload->SendRTP(buf, len))
    {
        return -1;
    }
    return 0;
}

int rtp_upload_sdk_destroy()
{
    if (0 != p_rtp_upload->Destroy())
    {
        return -1;
    }
    SAFE_DELETE(p_rtp_upload);
    return 0;
}

