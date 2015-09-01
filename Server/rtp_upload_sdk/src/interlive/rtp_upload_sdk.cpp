#include "util/WinHttpClient.h"
#include "rtp_upload_sdk.h"
#include "streamid.h"
#include "DXThread.h"
#include "util/util.h"
#include "rtp_trans/rtp_trans_manager.h"
#include <stddef.h>
#include <string>

#include "util/log.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#endif

using namespace std;

static Sender                    *p_Sender = NULL;
static Receiver                  *p_Receiver = NULL;
static RTPTransTimer             *p_RTPTransTimer = NULL;
static RTPTransManager           *p_RTPMgr = NULL;

static string receiver_ip;
static string receiver_tcp_port;
static string receiver_udp_port;

#if (defined __cplusplus)
extern "C" DLLEXPORT int rtp_upload_sdk_init(string ip, string tcp_port, string udp_port);
extern "C" DLLEXPORT int put_rtp(const char *buf, uint16_t len);
extern "C" DLLEXPORT int rtp_upload_sdk_destroy();
#endif

#define FIXED_SDP_STR "v=0\r\no=- 0 0 IN IP4 127.0.0.1\r\ns=000000000000000000000000152e9738\r\nc=IN IP4 10.10.69.127\r\nt=0 0\r\na=tool:YouKu Media Server\r\nm=audio 5002 RTP/AVP 97\r\na=rtpmap:97 MPEG4-GENERIC/48000/2\r\na=fmtp:97 profile-level-id=1;mode=AAC-hbr;sizelength=13;indexlength=3;indexdeltalength=3; config=1190\r\nm=video 5000 RTP/AVP 96\r\na=rtpmap:96 H264/90000"
int put_sdp()
{
    WinHttpClient client(L"http://10.10.69.127:7087/upload/sdp/000000000000000000000000152e9738.sdp?token=98765");

    // Set post data.
    string data = FIXED_SDP_STR;
    client.SetAdditionalDataToSend((BYTE *)data.c_str(), data.size());

    // Set request headers.
    wchar_t szSize[50] = L"";
    swprintf_s(szSize, L"%d", data.size());
    wstring headers = L"Content-Length: ";
    headers += szSize;
    headers += L"\r\nContent-Type: application/x-www-form-urlencoded\r\n";
    client.SetAdditionalRequestHeaders(headers);

    // Send http post request.
    client.SendHttpRequest(L"POST");

    wstring httpResponseHeader = client.GetResponseHeader();
    wstring httpResponseContent = client.GetResponseContent();
    return 0;
}

int rtp_upload_sdk_init(string ip, string tcp_port, string udp_port)
{
    LOGER::open();
    fprintf(LOGER::fp, "open\n");
    fflush(LOGER::fp);
    put_sdp();
    
    StreamId_Ext sid;
    sid.parse("000000000000000000000000152e9738");
    p_RTPMgr = new RTPTransManager("cctv", sid);
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

