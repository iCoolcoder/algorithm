#ifndef RTP_CONFIG_HEADER
#define RTP_CONFIG_HEADER
#include "stdint.h"
#include <string.h>

#define LIVESTREAMSDK_EXPORTS
#ifndef  LIVESTREAMSDK_EXPORTS
#include "../config_manager.h"
#include "../../util/log.h"
#endif
#define NACK_LIST_MAX_SIZE 250
#define NACK_PKT_MAX_AGE 450
#define SR_RR_INERVAL 5000
#define NACK_PROCESS_INERVAL 10
#define RTT_INERVAL 10
#define RTP_SESSION_TIMEOUT 10000
#define MAX_NACK_BITRATE 1 << 23



#ifndef  LIVESTREAMSDK_EXPORTS
class RTPTransConfig : public ConfigModule
#else
class RTPTransConfig
#endif
{
private:
    bool inited;
    char _module_name[256];

public:
    uint32_t max_nack_bandwidth_limit;
    uint32_t nack_interval;
    //receiver
    uint32_t max_nacklst_size;
    uint32_t max_packet_age;
    uint32_t sr_rr_interval;
    uint32_t rtt_detect_interval;
    uint32_t session_timeout;

    int32_t max_buffer_duration_ms;
    uint16_t rtp_port;

public:
    RTPTransConfig() {
        inited = false;
        set_module_name("rtp");
        set_default_config();
    }
    virtual ~RTPTransConfig() {

    }
    RTPTransConfig& operator=(const RTPTransConfig& rhv) {
        memmove(this, &rhv, sizeof(RTPTransConfig));
    }
    virtual void set_default_config() {
        max_nack_bandwidth_limit = MAX_NACK_BITRATE;
        nack_interval = NACK_PROCESS_INERVAL;
        max_nacklst_size = NACK_LIST_MAX_SIZE;
        max_packet_age = NACK_PKT_MAX_AGE;
        sr_rr_interval = SR_RR_INERVAL;
        rtt_detect_interval = RTT_INERVAL;
        session_timeout = RTP_SESSION_TIMEOUT;
    }

#ifndef  LIVESTREAMSDK_EXPORTS
    virtual bool load_config(xmlnode* xml_config) {
        //xmlnode *p = xmlgetchild(xml_config, _module_name, 0);
        xmlnode *p = xml_config;
        const char *q = NULL;
        if (p)
        {
            p = xmlgetchild(p, "rtp", 0);

            q = xmlgetattr(p, "max_nack_bandwidth_limit");
            if (q)
            {
                max_nack_bandwidth_limit = atoi(q);
            }

            q = xmlgetattr(p, "nack_interval");
            if (q)
            {
                nack_interval = atoi(q);
            }

            q = xmlgetattr(p, "max_nacklst_size");
            if (q)
            {
                max_nacklst_size = atoi(q);
            }

            q = xmlgetattr(p, "max_packet_age");
            if (q)
            {
                max_packet_age = atoi(q);
            }

            q = xmlgetattr(p, "sr_rr_interval");
            if (q)
            {
                sr_rr_interval = atoi(q);
            }

            q = xmlgetattr(p, "rtt_detect_interval");
            if (q)
            {
                rtt_detect_interval = atoi(q);
            }

            q = xmlgetattr(p, "session_timeout");
            if (q)
            {
                session_timeout = atoi(q);
            }

        }
        return true;
    }
#endif
    virtual bool reload() const {
        return true;
    }

    void set_module_name(const char *module_name) {
        memset(_module_name, 0, 256);
        strcat(_module_name, module_name);
    }

    virtual const char* module_name() const {
        return _module_name;
    }
    virtual void dump_config() const {
#ifndef  LIVESTREAMSDK_EXPORTS
        INF("rtp trans config: " "max_nack_bandwidth_limit=%d nack_interval=%d max_nacklst_size=%d max_packet_age=%d sr_rr_interval=%d rtt_detect_interval=%d session_timeout=%d",
            max_nack_bandwidth_limit, nack_interval, max_nacklst_size, max_packet_age, sr_rr_interval, rtt_detect_interval, session_timeout);
#endif
    }
};
#endif
