/**
* @file		streamid.h
* @brief	This file defines stream id format for interlive \n
* @author	songshenyi
* <pre><b>copyright: Youku</b></pre>
* <pre><b>email: </b>songshenyi@youku.com</pre>
* <pre><b>company: </b>http://www.youku.com</pre>
* <pre><b>All rights reserved.</b></pre>
* @date 2014/07/22
*
*/

#ifndef __STREAMID_H__
#define __STREAMID_H__

#include <common/type_defs.h>
#include <stdio.h>
#include <cstring>
#include <string>

#if __GNUC__ > 3
#include <tr1/unordered_map>
#endif

#ifdef _WIN32
#include <hash_map>
#else
#include <ext/hash_map>
#include <uuid/uuid.h>
#include "util/city.h" 
#endif

#ifdef _WIN32
#define NULL 0
#endif

static bool output_uuid = false;
static bool little_endian = false;

#ifdef _WIN32
#pragma pack(1)
class StreamId_Ext
{
public:
    StreamId_Ext()
    {
        id_uint64[0] = 0;
        id_uint64[1] = 0;
    }

    StreamId_Ext(const StreamId_Ext& stream_id)
    {
        id_uint64[0] = stream_id.id_uint64[0];
        id_uint64[1] = stream_id.id_uint64[1];
    }

    StreamId_Ext& operator=(const unsigned& input_id)
    {
        id_uint64[0] = 0;
        id_uint64[1] = 0;

        stream_id_32 = input_id;
        return *this;
    }

    bool operator==(const StreamId_Ext& right)
    {
        if (id_uint64[0] == right.id_uint64[0]
            && id_uint64[1] == right.id_uint64[1])
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool operator!=(const StreamId_Ext& right)
    {
        if (id_uint64[0] == right.id_uint64[0]
            && id_uint64[1] == right.id_uint64[1])
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    StreamId_Ext& operator=(const StreamId_Ext& right)
    {
        id_uint64[0] = right.id_uint64[0];
        id_uint64[1] = right.id_uint64[1];
        return *this;
    }

    int parse(const char* str, int radix = 10)
    {
        if (strlen(str) == 32)
        {
            if (little_endian)
            {
                for (int i = 0; i < 16; i++)
                {
                    int ii;
                    sscanf(str + 2 * i, "%2x", &ii);
                    id_char[i] = ii;
                }
            }
            else
            {
                for (int i = 0; i < 16; i++)
                {
                    int ii;
                    sscanf(str + 2 * i, "%2x", &ii);
                    id_char[15 - i] = ii;
                }
            }
        }
        else
        {
            int id_32 = strtol(str, (char **)NULL, radix);
            id_uint64[0] = 0;
            id_uint64[1] = 0;
            stream_id_32 = id_32;
        }

        return 0;
    }

    int parse(std::string& str)
    {
        return parse(str.c_str());
    }

    char* unparse(char* str) const
    {
        if (little_endian)
        {
            for (int i = 0; i < 16; i++)
            {
                sprintf(((char*)str) + i * 2, "%02x", id_char[i]);
            }
        }
        else
        {
            for (int i = 0; i < 16; i++)
            {
                sprintf(((char*)str) + i * 2, "%02x", id_char[15 - i]);
            }
        }

        return str;
    }

    std::string unparse() const
    {
        char str[40];
        unparse(str);
        return std::string(str);
    }

    unsigned get_32bit_stream_id() const
    {
        return stream_id_32;
    }

public:
    static unsigned get_32bit_stream_id(StreamId_Ext& id)
    {
        return id.stream_id_32;
    }

public:
    union
    {
        struct
        {
            unsigned stream_id_32;
            unsigned char  placeholder[12];
        };
        struct
        {
            unsigned char id_char[16];
        };
        struct
        {
            unsigned long long id_uint64[2];
        };
    };

};
#pragma pack()
#else

#pragma pack(1)
class StreamId_Ext
{
public:
    StreamId_Ext()
    {
        uuid_clear(uuid);
    }

    StreamId_Ext(uint32_t input_id, uint8_t iunput_rate_type = 0)
    {
        uuid_clear(uuid);
        stream_id_32 = input_id;
        rate_type = iunput_rate_type;
    }

    StreamId_Ext(const StreamId_Ext& stream_id)
    {
        id_uint64[0] = stream_id.id_uint64[0];
        id_uint64[1] = stream_id.id_uint64[1];
    }

    StreamId_Ext(const uuid_t& input_uuid)
    {
        uuid_copy(uuid, input_uuid);
    }

    StreamId_Ext& operator=(const uint32_t& input_id)
    {
        uuid_clear(uuid);
        stream_id_32 = input_id;
        return *this;
    }

    StreamId_Ext& operator=(const uuid_t& input_uuid)
    {
        uuid_copy(uuid, input_uuid);
        return *this;
    }

    StreamId_Ext& operator=(const StreamId_Ext& right)
    {
        id_uint64[0] = right.id_uint64[0];
        id_uint64[1] = right.id_uint64[1];
        return *this;
    }

    int32_t parse(const char* str, int radix = 10)
    {
        if (strlen(str) == 36)
        {
            int ret = uuid_parse(str, uuid);
            if (ret < 0)
            {
                return ret;
            }
        }
        else if (strlen(str) == 32)
        {
            if (little_endian)
            {
                for (int i = 0; i < 16; i++)
                {
                    int ii;
                    sscanf(str + 2 * i, "%2x", &ii);
                    id_char[i] = ii;
                }
            }
            else
            {
                for (int i = 0; i < 16; i++)
                {
                    int ii;
                    sscanf(str + 2 * i, "%2x", &ii);
                    id_char[15 - i] = ii;
                }
            }
        }
        else
        {
            int id_32 = strtol(str, (char **)NULL, radix);
            uuid_clear(uuid);
            stream_id_32 = id_32;
        }

        return 0;
    }

    bool operator<(const StreamId_Ext& right) const
    {
        return (uuid_compare(uuid, right.uuid) < 0);
    }

    bool operator>(const StreamId_Ext& right) const
    {
        return (uuid_compare(uuid, right.uuid) > 0);
    }

    bool operator==(const StreamId_Ext& right) const
    {
        return (uuid_compare(uuid, right.uuid) == 0);
    }

    bool operator!=(const StreamId_Ext& right) const
    {
        return (uuid_compare(uuid, right.uuid) != 0);
    }

    int parse(std::string& str)
    {
        return parse(str.c_str());
    }

    char* unparse(char* str) const
    {
        if (output_uuid)
        {
            uuid_unparse(uuid, str);
        }
        else
        {
            if (little_endian)
            {
                for (int i = 0; i < 16; i++)
                {
                    sprintf(((char*)str) + i * 2, "%02x", id_char[i]);
                }
            }
            else
            {
                for (int i = 0; i < 16; i++)
                {
                    sprintf(((char*)str) + i * 2, "%02x", id_char[15 - i]);
                }
            }
        }

        return str;
    }

    std::string unparse() const
    {
        char str[40];
        unparse(str);
        return std::string(str);
    }

    uint32_t get_32bit_stream_id() const
    {
        return stream_id_32;
    }

public:
    static uint32_t get_32bit_stream_id(StreamId_Ext& id)
    {
        return id.stream_id_32;
    }

    static uint8_t get_rate_type(StreamId_Ext& id)
    {
        return id.rate_type;
    }

public:
    union
    {
        uuid_t uuid;
        struct
        {
            uint32_t stream_id_32;
            uint8_t	rate_type;
            uint8_t placeholder[11];
        };
        struct
        {
            uint8_t id_char[16];
        };
        struct
        {
            uint64_t id_uint64[2];
        };
    };

};
#pragma pack()

namespace std
{
    template <>
    class equal_to<StreamId_Ext>
    {
    public:
        bool operator()(const StreamId_Ext& left, const StreamId_Ext& right) const
        {
            return (left == right);
        }
    };

#if __GNUC__ > 3
    namespace tr1
    {
        template <>
        class hash<StreamId_Ext>
        {
        public:
            size_t operator()(const StreamId_Ext& key) const
            {
                size_t hash_num = CityHash64((char*)(&key), sizeof(key));
                return hash_num;
            }
        };
    }
#endif
}

namespace __gnu_cxx
{
    template <>
    class hash<StreamId_Ext>
    {
    public:
        size_t operator()(const StreamId_Ext& key) const
        {
            size_t hash_num = CityHash64((char*)(&key), sizeof(key));
            return hash_num;
        }
    };
}

#endif

#endif /* __STREAMID_H__ */

