#ifdef _WIN32
#include "util/WinHttpClient.h"
#endif

#include "rtp_upload_sdk.h"
#include "rtp_info.h"
#include "streamid.h"
#include "worker.h"
#include "util/util.h"
#include "util/buffer.h"
#include "avformat/sdp.h"
#include "common/proto_define.h"
#include "common/proto_define_rtp.h"
#include "common/proto_rtp_rtcp.h"
#include "rtp_trans/rtp_trans_manager.h"
#include <stddef.h>
#include <string>
#include "util/httpclient.h"

#include "util/log.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#endif

using namespace std;
using namespace avformat;

static Sender                    *p_Sender = NULL;
static Receiver                  *p_Receiver = NULL;
static RTPTransTimer             *p_RTPTransTimer = NULL;
static RTPTransManager           *p_RTPMgr = NULL;

static string receiver_ip;
static string receiver_tcp_port;
static string receiver_udp_port;
static StreamId_Ext stream_id;

#if (defined __cplusplus)
extern "C" DLLEXPORT int rtp_upload_sdk_init(string ip, string tcp_port, string udp_port, SessionDescription_EXT &sd);
extern "C" DLLEXPORT int put_session_description(SessionDescription_EXT &sd);
extern "C" DLLEXPORT int put_request();
extern "C" DLLEXPORT int put_rtp(const char *buf, uint16_t len);
extern "C" DLLEXPORT int rtp_upload_sdk_destroy();
#endif

#define FIXED_SDP_STR "v=0\r\no=- 0 0 IN IP4 127.0.0.1\r\ns=000000000000000000000000152e9738\r\nc=IN IP4 10.10.69.127\r\nt=0 0\r\na=tool:YouKu Media Server\r\nm=audio 5002 RTP/AVP 97\r\na=rtpmap:97 MPEG4-GENERIC/48000/2\r\na=fmtp:97 profile-level-id=1;mode=AAC-hbr;sizelength=13;indexlength=3;indexdeltalength=3; config=1190\r\nm=video 5000 RTP/AVP 96\r\na=rtpmap:96 H264/90000"

#ifdef _WIN32
wstring string2wstring(string str)
{
    wstring result;
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
    TCHAR* buffer = new TCHAR[len + 1];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
    buffer[len] = '\0';             
    result.append(buffer);
    delete[] buffer;
    return result;
}
#endif

int put_sdp(string sdp, string sid)
{
    char url[1024] = { 0 };
    sprintf(url, "http://%s:%s/upload/sdp/%s.sdp?token=98765", 
        receiver_ip.c_str(), receiver_tcp_port.c_str(), sid.c_str());

    string url_str = url;
#ifdef _WIN32
    wstring url_wstr = string2wstring(url_str);
    WinHttpClient client(url_wstr);

    // Set post data.
    client.SetAdditionalDataToSend((BYTE *)sdp.c_str(), sdp.size());

    // Set request headers.
    wchar_t szSize[50] = L"";
    swprintf_s(szSize, L"%d", sdp.size());
    wstring headers = L"Content-Length: ";
    headers += szSize;
    headers += L"\r\nContent-Type: application/x-www-form-urlencoded\r\n";
    client.SetAdditionalRequestHeaders(headers);

    // Send http post request.
    client.SendHttpRequest(L"POST");

    wstring httpResponseHeader = client.GetResponseHeader();
    wstring httpResponseContent = client.GetResponseContent();
#else
    CHttpClient hc;
    string rsp;
    hc.Post(url_str, sdp, rsp); 
#endif
    return 0;
}

int rtp_upload_sdk_init(string ip, string tcp_port, string udp_port, SessionDescription_EXT &sd)
{
    receiver_ip = ip;
    receiver_tcp_port = tcp_port;
    receiver_udp_port = udp_port;
    stream_id.parse(sd.stream_id);

    LOGER::open();
    fprintf(LOGER::fp, "open\n");
    fflush(LOGER::fp);
    
    StreamId_Ext sid;
    sid.parse(sd.stream_id.c_str());

    p_RTPMgr = new RTPTransManager("channel_name", sid);
    if (NULL == p_RTPMgr) {
        return -1;
    }
    if (0 != p_RTPMgr->open_sender()) {
        return -1;
    }
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    p_Sender = new Sender(fd, ip, udp_port, p_RTPMgr);
    if (NULL == p_Sender) {
        return -1;
    }
    p_Receiver = new Receiver(fd, p_RTPMgr);
    if (NULL == p_Receiver) {
        return -1;
    }

    p_RTPTransTimer = new RTPTransTimer(p_RTPMgr);
    if (NULL == p_RTPTransTimer) {
        return -1;
    }

#ifdef _WIN32
    WSADATA wsa = { 0 };
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
    p_Sender->Start();
    p_Receiver->Start();
    p_RTPTransTimer->Start();
    p_Sender->Join();
    p_Receiver->Join();
    p_RTPTransTimer->Join();
    return 0;
}

int put_session_description(SessionDescription_EXT &sd)
{
    SessionDescription temp;
    temp.start_time = sd.start_time;
    temp.end_time = sd.end_time;
    temp.stream_id = sd.stream_id;
    temp.receiver_ip = sd.receiver_ip;
    
    H264MediaInfo* h264_media_info = new H264MediaInfo();
    h264_media_info->clock_rate = sd.h264_media_info->clock_rate;
    h264_media_info->payload_type = sd.h264_media_info->payload_type;
    h264_media_info->transport = sd.h264_media_info->transport;
    h264_media_info->port = sd.h264_media_info->port;
    h264_media_info->ssrc = sd.h264_media_info->ssrc;
    AACMediaInfo* aac_media_info = new AACMediaInfo();
    aac_media_info->clock_rate = sd.aac_media_info->clock_rate;
    aac_media_info->audioObjectType = sd.aac_media_info->audioObjectType;
    aac_media_info->channels = sd.aac_media_info->channels;
    aac_media_info->ssrc = sd.aac_media_info->ssrc;
    aac_media_info->payload_type = sd.aac_media_info->payload_type;
    aac_media_info->transport = sd.aac_media_info->transport;
    aac_media_info->port = sd.aac_media_info->port;

    temp.media_list.push_back(aac_media_info);
    temp.media_list.push_back(h264_media_info);

    SdpInfo *sdp = temp.generate_sdp_session_level();
    sdp->generate_sdp_str();
    string sdp_str = sdp->get_sdp_str();

    put_sdp(sdp_str, temp.stream_id);

    SAFE_DELETE(aac_media_info);
    SAFE_DELETE(h264_media_info);
    SAFE_DELETE(sdp);
    return 0;
}

int put_request()
{
    rtp_u2r_req_state req;
    req.payload_type = 2;
    memcpy(req.streamid, &stream_id, sizeof(req.streamid));
    memset(req.token, 0, sizeof(req.token));
    char *token = "98765";
    memcpy(req.token, token, strlen(token));
    req.version = 1;
    req.user_id = 2;
    buffer *obuf = buffer_create_max(65536, 65536);
    encode_rtp_u2r_req_state(&req, obuf);

    p_RTPMgr->send_request(buffer_data_ptr(obuf), buffer_data_len(obuf));
    buffer_free(obuf);
    return 0;
}

int put_rtp(const char *buf, uint16_t len)
{
    p_RTPMgr->put_rtp(buf, len);
    return 0;
}

int rtp_upload_sdk_destroy()
{
#ifdef _WIN32
    WSACleanup();
#endif
    SAFE_DELETE(p_RTPMgr);
    SAFE_DELETE(p_Sender);
    SAFE_DELETE(p_Receiver);
    SAFE_DELETE(p_RTPTransTimer);
    return 0;
}

