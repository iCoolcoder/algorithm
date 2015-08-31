#pragma once

#ifdef _WIN32
#include <time.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#else
#include <sys/time.h>
#endif
#include <stdint.h>

typedef uint16_t proto_t;
typedef uint16_t f2t_v2_seq_t;
#define STREAM_ID_LEN 16

enum
{
	//forward <----> forward v1
	// used in forward_server and forward_client.
	//1~24
	CMD_FC2FS_REQ_STREAM = 1,
	CMD_FS2FC_RSP_STREAM = 2,
	CMD_FS2FC_STREAMING_HEADER = 3,
	CMD_FS2FC_STREAMING = 4,
	CMD_FC2FS_KEEPALIVE = 5,
	CMD_FC2FS_UNREQ_STREAM = 6,

	//forward <----> forward v2
	// used in forward_server_v2 and forward_client_v2, but only support short stream id,
	// already replaced by f2f v3.
	CMD_FC2FS_START_TASK = 7,
	CMD_FS2FC_START_TASK_RSP = 8,
	CMD_FS2FC_STREAM_DATA = 9,
	CMD_F2F_STOP_TASK = 10,
	//CMD_F2F_STOP_TASK_RSP = 11,

	//forward <----> forward v3
	// still used in forward_server_v2 and forward_clinet_v2, support long stream id.
	//25~49
	CMD_FC2FS_START_TASK_V3 = 25,
	CMD_F2F_STOP_TASK_V3 = 26,
	CMD_FS2FC_START_TASK_RSP_V3 = 27,
	CMD_FS2FC_STREAM_DATA_V3 = 28,

	//forward <----> portal
	//50~99
	CMD_F2P_KEEPALIVE = 50,
	CMD_P2F_INF_STREAM = 51,
	CMD_P2F_START_STREAM = 52,
	CMD_P2F_CLOSE_STREAM = 53,
    CMD_P2F_INF_STREAM_V2 = 54,
    CMD_P2F_START_STREAM_V2 = 55,

	//receiver <----> portal
	//100~149
	CMD_R2P_KEEPALIVE = 100,

	//uploader<--->up_sche
	//150~200
	CMD_U2US_REQ_ADDR = 150,
	CMD_US2U_RSP_ADDR = 151,

	//tracker<--->forward
	//200~249
	CMD_F2T_REGISTER_REQ = 200,
	CMD_F2T_REGISTER_RSP = 201,
	CMD_F2T_ADDR_REQ = 202,
	CMD_F2T_ADDR_RSP = 203,
	CMD_F2T_UPDATE_STREAM_REQ = 204,
	CMD_F2T_UPDATE_STREAM_RSP = 205,
	CMD_F2T_KEEP_ALIVE_REQ = 206,
	CMD_F2T_KEEP_ALIVE_RSP = 207,

    //tracker<--->forward v2
    //250~299
    CMD_FS2T_REGISTER_REQ_V2 = 250,
    CMD_FS2T_REGISTER_RSP_V2 = 251,
    CMD_FC2T_ADDR_REQ_V2 = 252,
    CMD_FC2T_ADDR_RSP_V2 = 253,
    CMD_FS2T_UPDATE_STREAM_REQ_V2 = 254,
    CMD_FS2T_UPDATE_STREAM_RSP_V2 = 255,
    CMD_FS2T_KEEP_ALIVE_REQ_V2 = 256,
    CMD_FS2T_KEEP_ALIVE_RSP_V2 = 257,

    //tracker<--->forward v3 (support long stream id)
    //250~299
    CMD_FS2T_REGISTER_REQ_V3 = 270,
    CMD_FS2T_REGISTER_RSP_V3 = 271,
    CMD_FC2T_ADDR_REQ_V3 = 272,
    CMD_FC2T_ADDR_RSP_V3 = 273,
    CMD_FS2T_UPDATE_STREAM_REQ_V3 = 274,
    CMD_FS2T_UPDATE_STREAM_RSP_V3 = 275,
    CMD_FS2T_KEEP_ALIVE_REQ_V3 = 276,
    CMD_FS2T_KEEP_ALIVE_RSP_V3 = 277,

    //server stat
    CMD_IC2T_SERVER_STAT_REQ = 298,
    CMD_IC2T_SERVER_STAT_RSP = 299,
	//receiver<--->up_sche
	//300~350
	CMD_US2R_REQ_UP = 300,
	CMD_R2US_RSP_UP = 301,
	CMD_R2US_KEEPALIVE = 302,

	//uploader<--->receiver
	//350~399
	CMD_U2R_REQ_STATE = 350,
	CMD_U2R_RSP_STATE = 351,
	CMD_U2R_STREAMING = 352,
	CMD_U2R_CMD = 353,

	CMD_U2R_REQ_STATE_V2 = 354,
	CMD_U2R_RSP_STATE_V2 = 355,
	CMD_U2R_STREAMING_V2 = 356,
	CMD_U2R_CMD_V2 = 357,

    // mod_tracker  <--> tracker
    CMD_MT2T_REQ_RSP = 400,

    // rtp uploader <----> receiver
    CMD_RTP_U2R_REQ_STATE = 500,
    CMD_RTP_U2R_RSP_STATE = 501,
    CMD_RTP_U2R_PACKET = 502,
    CMD_RTCP_U2R_PACKET = 503,

    // rtp downloader <----> player
    CMD_RTP_D2P_REQ_STATE = 600,
    CMD_RTP_D2P_RSP_STATE = 601,
    CMD_RTP_D2P_PACKET = 602,
    CMD_RTCP_D2P_PACKET = 603,

    // rtp forward <----> forward
    CMD_RTP_F2F_REQ_STATE = 700,
    CMD_RTP_F2F_START_STREAM = 701, // fc2fs
    CMD_RTP_F2F_STOP_STREAM = 702, // fc2fs
    CMD_RTP_F2F_RESULT = 703, // fs2fc/fc2fs
    CMD_RTP_F2F_NORM_DATA = 704, // fs2fc/fc2fs

};

static const int32_t TRACKER_RSP_RESULT_OK = 0;
static const int32_t TRACKER_RSP_RESULT_BADREQ = 1;
static const int32_t TRACKER_RSP_RESULT_NORESULT = 2;
static const int32_t TRACKER_RSP_RESULT_INNER_ERR = 3;

static const int32_t FORWARD_RSP_RESULT_OK = 0;
static const int32_t FORWARD_RSP_RESULT_BADREQ = 1;
static const int32_t FORWARD_RSP_RESULT_NORESULT = 2;
static const int32_t FORWARD_RSP_RESULT_INNER_ERR = 3;

static const int32_t FORWARD_SESSION_PROG_START = 0;
static const int32_t FORWARD_SESSION_PROG_IN_PROGRESS = 1;
static const int32_t FORWARD_SESSION_PROG_NOT_DATA = 2;
static const int32_t FORWARD_SESSION_PROG_BADREQ = 3;
static const int32_t FORWARD_SESSION_PROG_INNER_ERR = 4;
static const int32_t FORWARD_SESSION_PROG_WAIT = 5;//Need to wait for the data
static const int32_t FORWARD_SESSION_PROG_END = 6;

typedef enum
{
	UPD_CMD_DEL = 0,
	UPD_CMD_ADD
}update_cmd;

typedef enum
{
    PAYLOAD_TYPE_MEDIA_BEGIN = 0,
    PAYLOAD_TYPE_FLV = 0,
    PAYLOAD_TYPE_MPEGTS = 1,

    PAYLOAD_TYPE_MEDIA_END = 31,
    PAYLOAD_TYPE_HEARTBEAT = 32,
}payload_type;

#pragma pack(1)
typedef struct proto_header
{
	uint8_t magic;
	uint8_t version;
	uint16_t cmd;
	uint32_t size;
}proto_header;

typedef struct media_header
{
	uint32_t streamid;
	uint32_t payload_size;
	uint8_t payload_type;
	uint8_t payload[0];
}media_header;

#pragma pack()

