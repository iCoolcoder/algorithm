#include "proto_rtp_rtcp.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "proto.h"
#include "util/util.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#else
#include <arpa/inet.h>
#endif



#define MAKE_BUF_USABLE(obuf, total_sz) \
{\
    int ret = 0; \
if (buffer_capacity(obuf) < total_sz)\
{\
    ret = buffer_expand_capacity(obuf, total_sz); \
if (0 != ret)\
    return ret; \
}\
}

int decode_rtp_proto(uint8_t* output, uint8_t* input, RTPProtoType& type, uint16_t& len)
{
    if (input == NULL || input[0] != '$')
    {
        return -1;
    }

    rtp_common_header* header = (rtp_common_header*)(input);
    type = (RTPProtoType)(header->rtp_proto_type);

    switch (type)
    {
    case PROTO_RTP:
    case PROTO_RTCP:
    {
                       rtp_proto_header* header = (rtp_proto_header*)input;
                       header->common_header.payload_len = ntohs(header->common_header.payload_len);
                       len = header->common_header.payload_len;
                       output = header->payload;
                       return type;
    }
        break;

    default:
        break;
    }

    len = 0;
    return -1;
}

int decode_rtp_streamid_proto(uint8_t* output, uint8_t* input, uint8_t* stream_id, RTPProtoType& type, uint16_t& len)
{
    if (input == NULL || input[0] != '$')
    {
        return -1;
    }

    rtp_common_header* header = (rtp_common_header*)(input);
    type = (RTPProtoType)(header->rtp_proto_type);

    switch (type)
    {
    case PROTO_RTP_STREAMID:
    case PROTO_RTCP_STREAMID:
    {
                                rtp_streamid_proto_header* header = (rtp_streamid_proto_header*)input;
                                header->common_header.payload_len = ntohs(header->common_header.payload_len);
                                len = header->common_header.payload_len;
                                memcpy(stream_id, header->streamid, STREAM_ID_LEN);
                                output = header->payload;
                                return type;
    }
        break;

    default:
        break;
    }

    len = 0;
    return -1;
}

int decode_rtp_cmd_proto(uint8_t* output, uint8_t* input, uint8_t* stream_id, RTPProtoType& type, uint16_t& len)
{
    if (input == NULL || input[0] != '$')
    {
        return -1;
    }

    rtp_common_header* header = (rtp_common_header*)(input);
    type = (RTPProtoType)(header->rtp_proto_type);

    switch (type)
    {
    case PROTO_CMD:
    {
                      rtp_cmd_proto_header* header = (rtp_cmd_proto_header*)input;
                      header->common_header.payload_len = ntohs(header->common_header.payload_len);
                      len = header->common_header.payload_len;
                      output = header->payload;
                      return type;
    }
        break;

    default:
        break;
    }

    len = 0;
    return -1;
}

int encode_rtp_proto_uint8(uint8_t* buffer, uint16_t& len, RTPProtoType type, const uint8_t* payload, uint16_t data_len)
{
    if (buffer == NULL || payload == NULL || data_len == 0)
    {
        return -1;
    }

    switch (type)
    {
    case PROTO_RTP:
    case PROTO_RTCP:
    {
                       rtp_proto_header* header = (rtp_proto_header*)buffer;
                       header->common_header.flag = '$';
                       header->common_header.rtp_proto_type = type;
                       header->common_header.payload_len = htons(data_len);
                       memcpy(header->payload, payload, data_len);
                       len = data_len + 4;
                       return 0;
    }
        break;
    default:
        break;
    }

    len = 0;
    return -1;
}


int encode_rtp_streamid_proto_uint8(uint8_t* buffer, uint16_t& len, RTPProtoType type, uint8_t* stream_id, const uint8_t* payload, uint16_t data_len)
{
    if (buffer == NULL || payload == NULL || data_len == 0)
    {
        return -1;
    }

    switch (type)
    {
    case PROTO_RTP:
    case PROTO_RTCP:
    {
                                if (stream_id == NULL)
                                {
                                    return -1;
                                }

                                rtp_streamid_proto_header* header = (rtp_streamid_proto_header*)buffer;
                                header->common_header.flag = '$';
                                header->common_header.rtp_proto_type = type;
                                header->common_header.payload_len = htons(data_len);
                                memcpy(header->streamid, stream_id, STREAM_ID_LEN);
                                memcpy(header->payload, payload, data_len);
                                len = data_len + STREAM_ID_LEN + 4;
                                return 0;
    }
        break;
    default:
        break;
    }

    len = 0;
    return -1;
}

int encode_rtp_cmd_proto_uint8(uint8_t* buffer, uint16_t& len, const uint8_t* payload, uint16_t data_len)
{
    if (buffer == NULL || payload == NULL || data_len == 0)
    {
        return -1;
    }

    RTPProtoType type = PROTO_CMD;

    rtp_cmd_proto_header* header = (rtp_cmd_proto_header*)buffer;
    header->common_header.flag = '$';
    header->common_header.rtp_proto_type = type;
    header->common_header.payload_len = htons(data_len);
    memcpy(header->payload, payload, data_len);
    len = data_len + 4;
    return 0;
}

int encode_rtp_proto(buffer* buf, RTPProtoType type, const uint8_t* payload, uint16_t data_len)
{
    if (buf == NULL || payload == NULL || data_len == 0)
    {
        return -1;
    }

    switch (type)
    {
    case PROTO_RTP:
    case PROTO_RTCP:
    {

                               buffer_append_byte(buf, '$');
                               buffer_append_byte(buf, (uint8_t)type);

                               data_len = htons(data_len);
                               buffer_append_ptr(buf, &data_len, sizeof(data_len));
                               buffer_append_ptr(buf, payload, data_len);

                               return 0;
    }
        break;
    default:
        break;
    }

    return -1;
}

int encode_rtp_streamid_proto(buffer* buf, RTPProtoType type, uint8_t* stream_id, const uint8_t* payload, uint16_t data_len)
{
    if (buf == NULL || payload == NULL || data_len == 0)
    {
        return -1;
    }

    switch (type)
    {
    case PROTO_RTP_STREAMID:
    case PROTO_RTCP_STREAMID:
    {
                               if (stream_id == NULL)
                               {
                                   return -1;
                               }

                               buffer_append_byte(buf, '$');
                               buffer_append_byte(buf, (uint8_t)type);

                               data_len = htons(data_len);
                               buffer_append_ptr(buf, &data_len, sizeof(data_len));
                               buffer_append_ptr(buf, stream_id, STREAM_ID_LEN);
                               buffer_append_ptr(buf, payload, data_len);

                               return 0;
    }
        break;
    default:
        break;
    }

    return -1;
}

int encode_rtp_cmd_proto(buffer* buf, const uint8_t* payload, uint16_t data_len)
{
    if (buf == NULL || payload == NULL || data_len == 0)
    {
        return -1;
    }

    RTPProtoType type = PROTO_CMD;
    
    buffer_append_byte(buf, '$');
    buffer_append_byte(buf, (uint8_t)type);

    data_len = htons(data_len);
    buffer_append_ptr(buf, &data_len, sizeof(data_len));
    buffer_append_ptr(buf, payload, data_len);

    return 0;
}

int decode_rtp_u2r_req_state(uint8_t* input, uint16_t len, rtp_u2r_req_state* body)
{
    assert(len >= sizeof(proto_header)+sizeof(rtp_u2r_req_state));
    size_t min_sz = sizeof(proto_header)+sizeof(rtp_u2r_req_state);
    proto_header h;

    if (0 != decode_header_uint8(input, len, &h))
        return -1;
    if (len < min_sz || h.size < min_sz)
        return -2;
    body = (rtp_u2r_req_state*)(input + sizeof(proto_header));

    body->version = ntohl(body->version);
    body->user_id = util_ntohll(body->user_id);

    return 0;
}

int decode_rtp_d2p_req_state(uint8_t* input, uint16_t len, rtp_d2p_req_state* body)
{
    assert(len >= sizeof(proto_header)+sizeof(rtp_d2p_req_state));
    size_t min_sz = sizeof(proto_header)+sizeof(rtp_d2p_req_state);
    proto_header h;

    if (0 != decode_header_uint8(input, len, &h))
        return -1;
    if (len < min_sz || h.size < min_sz)
        return -2;
    body = (rtp_d2p_req_state*)(input + sizeof(proto_header));

    body->version = ntohl(body->version);

    return 0;
}

int decode_rtp_f2f_req_state(uint8_t* input, uint16_t len, rtp_f2f_req_state* body)
{
    assert(len >= sizeof(proto_header)+sizeof(rtp_f2f_req_state));
    size_t min_sz = sizeof(proto_header)+sizeof(rtp_f2f_req_state);
    proto_header h;

    if (0 != decode_header_uint8(input, len, &h))
        return -1;
    if (len < min_sz || h.size < min_sz)
        return -2;
    body = (rtp_f2f_req_state*)(input + sizeof(proto_header));

    body->version = ntohl(body->version);

    return 0;
}

int encode_rtp_f2f_req_state(rtp_f2f_req_state* body, buffer* obuf)
{
    uint32_t total_sz = sizeof(proto_header)+sizeof(rtp_f2f_req_state);

    MAKE_BUF_USABLE(obuf, total_sz);
    encode_header(obuf, CMD_RTP_F2F_REQ_STATE, total_sz);
    
    uint32_t nversion = htonl(body->version);
    buffer_append_ptr(obuf, &nversion, sizeof(body->version));
    buffer_append_ptr(obuf, body->streamid, sizeof(body->streamid));
    return 0;
}

int encode_rtp_f2f_req_state_uint8(rtp_f2f_req_state* body, uint8_t* obuf, uint16_t& len)
{
    uint32_t total_sz = sizeof(proto_header)+sizeof(rtp_f2f_req_state);
    uint32_t offset;
    encode_header_uint8(obuf, offset, CMD_RTP_F2F_REQ_STATE, total_sz);

    uint32_t nversion = htonl(body->version);
    memcpy(obuf + offset, &nversion, sizeof(body->version));
    offset += sizeof(body->version);
    memcpy(obuf + offset, body->streamid, sizeof(body->streamid));
    offset += sizeof(body->streamid);

    len = offset;

    return 0;
}

int encode_rtp_u2r_rsp_state(const rtp_u2r_rsp_state* body, buffer* obuf)
{
    uint32_t total_sz = sizeof(proto_header)+sizeof(rtp_u2r_rsp_state);

    MAKE_BUF_USABLE(obuf, total_sz);
    encode_header(obuf, CMD_RTP_U2R_RSP_STATE, total_sz);

    uint32_t nversion = htonl(body->version);
    uint16_t nresult = htons(body->result);
    buffer_append_ptr(obuf, &nversion, sizeof(body->version));
    buffer_append_ptr(obuf, body->streamid, sizeof(body->streamid));
    buffer_append_ptr(obuf, &nresult, sizeof(body->result));

    return 0;
}

int encode_rtp_u2r_rsp_state_uint8(rtp_u2r_rsp_state* body, uint8_t* obuf, uint16_t& len)
{
    uint32_t total_sz = sizeof(proto_header)+sizeof(rtp_u2r_rsp_state);
    uint32_t offset;
    encode_header_uint8(obuf, offset, CMD_RTP_U2R_RSP_STATE, total_sz);

    uint32_t nversion = htonl(body->version);
    uint16_t nresult = htons(body->result);
    memcpy(obuf + offset, &nversion, sizeof(body->version));
    offset += sizeof(body->version);
    memcpy(obuf + offset, body->streamid, sizeof(body->streamid));
    offset += sizeof(body->streamid);
    memcpy(obuf + offset, &nresult, sizeof(body->result));
    offset += sizeof(body->result);

    len = offset;

    return 0;
}

int encode_rtp_d2p_rsp_state(rtp_d2p_rsp_state* body, buffer* obuf)
{
    uint32_t total_sz = sizeof(proto_header)+sizeof(rtp_d2p_rsp_state);

    MAKE_BUF_USABLE(obuf, total_sz);
    encode_header(obuf, CMD_RTP_D2P_RSP_STATE, total_sz);

    uint32_t nversion = htonl(body->version);
    uint16_t nresult = htons(body->result);
    buffer_append_ptr(obuf, &nversion, sizeof(body->version));
    buffer_append_ptr(obuf, body->streamid, sizeof(body->streamid));
    buffer_append_ptr(obuf, &nresult, sizeof(body->result));

    return 0;
}

int encode_rtp_d2p_rsp_state_uint8(rtp_d2p_rsp_state* body, uint8_t* obuf, uint16_t& len)
{
    uint32_t total_sz = sizeof(proto_header)+sizeof(rtp_u2r_rsp_state);
    uint32_t offset;
    encode_header_uint8(obuf, offset, CMD_RTP_D2P_RSP_STATE, total_sz);

    uint32_t nversion = htonl(body->version);
    uint16_t nresult = htons(body->result);
    memcpy(obuf + offset, &nversion, sizeof(body->version));
    offset += sizeof(body->version);
    memcpy(obuf + offset, body->streamid, sizeof(body->streamid));
    offset += sizeof(body->streamid);
    memcpy(obuf + offset, &nresult, sizeof(body->result));
    offset += sizeof(body->result);

    len = offset;

    return 0;
}

int 
decode_rtp_u2r_req_state(rtp_u2r_req_state * body, buffer * ibuf)
{
    assert(buffer_data_len(ibuf) >= sizeof(proto_header));
    size_t actual_sz = sizeof(proto_header)+sizeof(rtp_u2r_req_state);
    proto_header h;

    if (0 != decode_header(ibuf, &h))
        return -1;
    if (buffer_data_len(ibuf) < actual_sz || h.size < actual_sz)
        return -2;
    body = (rtp_u2r_req_state *)(buffer_data_ptr(ibuf)
        + sizeof(proto_header));

    body->version = ntohl(body->version);
    body->user_id = util_ntohll(body->user_id);
    return 0;
}

int 
encode_rtp_u2r_req_state(const rtp_u2r_req_state *body, buffer *obuf)
{
    if (body == NULL || obuf == NULL)
    {
        return -1;
    }

    uint32_t total_sz = sizeof(proto_header)+sizeof(rtp_u2r_req_state);
    encode_header(obuf, CMD_RTP_U2R_REQ_STATE, total_sz);

    uint32_t version = htonl(body->version);
    uint64_t user_id = util_htonll(body->user_id);

    buffer_append_ptr(obuf, &version, sizeof(version));
    buffer_append_ptr(obuf, &(body->streamid), sizeof(body->streamid));
    buffer_append_ptr(obuf, &user_id, sizeof(user_id));
    buffer_append_ptr(obuf, &body->token, sizeof(body->token));
    buffer_append_ptr(obuf, &body->payload_type, sizeof(uint8_t));

    return 0;
}

int 
encode_rtp_u2r_packet(uint8_t* body, uint32_t len, buffer * obuf)
{
    uint32_t total_sz = sizeof(proto_header)+len;

    MAKE_BUF_USABLE(obuf, total_sz);
    encode_header(obuf, CMD_RTP_U2R_PACKET, total_sz);
    buffer_append_ptr(obuf, body, len);
    return 0;
}

int
encode_rtcp_u2r_packet(uint8_t* body, uint32_t len, buffer * obuf)
{
    uint32_t total_sz = sizeof(proto_header)+len;

    MAKE_BUF_USABLE(obuf, total_sz);
    encode_header(obuf, CMD_RTCP_U2R_PACKET, total_sz);
    buffer_append_ptr(obuf, body, len);
    return 0;
}

int 
decode_rtp_d2p_req_state(rtp_d2p_req_state * body, buffer * ibuf)
{
    assert(buffer_data_len(ibuf) >= sizeof(proto_header)+sizeof(rtp_d2p_req_state));
    size_t min_sz = sizeof(proto_header)+sizeof(rtp_d2p_req_state);
    proto_header h;

    if (0 != decode_header(ibuf, &h))
        return -1;
    if (buffer_data_len(ibuf) < min_sz || h.size < min_sz)
        return -2;
    body = (rtp_d2p_req_state*)(buffer_data_ptr(ibuf) + sizeof(proto_header));
    body->version = ntohl(body->version);

    return 0;
}

int 
encode_rtp_d2p_packet(uint8_t * body, uint32_t len, buffer * obuf)
{
    uint32_t total_sz = sizeof(proto_header)+len;

    MAKE_BUF_USABLE(obuf, total_sz);
    encode_header(obuf, CMD_RTP_D2P_PACKET, total_sz);
    buffer_append_ptr(obuf, body, len);
    return 0;
}

int 
encode_rtcp_d2p_packet(uint8_t * body, uint32_t len, buffer * obuf)
{
    uint32_t total_sz = sizeof(proto_header)+len;

    MAKE_BUF_USABLE(obuf, total_sz);
    encode_header(obuf, CMD_RTCP_D2P_PACKET, total_sz);
    buffer_append_ptr(obuf, body, len);
    return 0;
}

int encode_f2f_rtp_packet_header(const f2f_rtp_packet_header * body, char* buf, size_t size)
{
    if (size < sizeof(f2f_rtp_packet_header))
        return -1;

    memmove(buf, body, sizeof(f2f_rtp_packet_header));
    return 0;
}

int decode_f2f_rtp_packet_header(f2f_rtp_packet_header * body, const char* buf, size_t size)
{
    if (size < sizeof(f2f_rtp_packet_header))
        return -1;

    memmove(body, buf, sizeof(f2f_rtp_packet_header));
    return 0;
}
