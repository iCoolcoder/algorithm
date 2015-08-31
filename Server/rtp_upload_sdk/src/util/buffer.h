/*****************************************************
* YueHonghui, 2013-06-23
* hhyue@tudou.com
* copyright:youku.com
* ported from QueHongyu
* **************************************************/
#ifndef BUFFER_H_
#define BUFFER_H_
#include <stddef.h>
#define inline __inline
#ifndef _WIN32
#include <unistd.h>
#endif
#include <string.h>

#if (defined __cplusplus && !defined _WIN32)
extern "C"
{
#endif
    struct buffer
    {
        char *ptr;
        size_t size, end, start, max;
    };

    typedef struct buffer buffer;


    enum BUFFER_ERROR_CODE
    {
        BUFFER_OVER_SIZE = -4,
        BUFFER_CAPACITY_ERROR = -3,
        BUFFER_MALLOC_ERROR = -2,
        BUFFER_SOCKET_ERROR = -1,
    };

    buffer *buffer_init(size_t size);
    buffer *buffer_create_max(size_t size, size_t max);
    void buffer_free(buffer * buf);
    int buffer_expand(buffer * buf, size_t total_sz);
    int buffer_expand_capacity(buffer * buf, size_t cap_sz);
    int buffer_copy(buffer * dst, buffer * src);
    int buffer_append(buffer * dst, buffer * src);
    int buffer_append_ptr(buffer * dst, const void *src, size_t src_len);
    int buffer_append_byte(buffer * dst, unsigned char byte);
    size_t buffer_eat(buffer * buf, size_t bytes);
    size_t buffer_ignore(buffer * buf, size_t bytes);
    //int buffer_read_fd(buffer * buf, int fd);
    //int buffer_read_fd_max(buffer * buf, int fd, size_t max);
    //int buffer_write_fd(buffer * buf, int fd);
    //int buffer_write_fd_max(buffer * buf, int fd, size_t max);

    //// for udp
    //int buffer_udp_recvfrom_fd(buffer *buf, int fd, struct sockaddr_in &client_addr);
    //int buffer_udp_recvfrom_fd_max(buffer *buf, int fd, size_t max, struct sockaddr_in &client_addr);

    static inline size_t
        buffer_size(const buffer * buf)
    {
            return buf->size;
        }

    static inline size_t
        buffer_max(const buffer * buf)
    {
            return buf->max;
        }

    static inline void
        buffer_reset(buffer * buf)
    {
            buf->start = buf->end = 0;
        }

    static inline void
        buffer_adjust(buffer * buf)
    {
            memmove(buf->ptr, buf->ptr + buf->start, buf->end - buf->start);
            buf->end = buf->end - buf->start;
            buf->start = 0;
        }

    static inline void
        buffer_try_adjust(buffer * buf)
    {
            if (buf->start == buf->end) {
                buf->start = buf->end = 0;
                return;
            }
            if (buf->size >= 128 * 1024 || buf->start >= 0.5 * buf->size) {
                buffer_adjust(buf);
            }
        }

    //int read_dvb_buffer (int, buffer*);
    static inline size_t
        buffer_data_len(const buffer * buf)
    {
            return buf->end - buf->start;
        }

    static inline size_t
        buffer_capacity(const buffer * buf)
    {
            return buf->size - buf->end;
        }

    static inline size_t
        buffer_free_size(const buffer * buf)
    {
            return buf->size - (buf->end - buf->start);
        }

    static inline const char *
        buffer_data_ptr(const buffer * buf)
    {
            return buf->ptr + buf->start;
        }

    static inline const char *
        buffer_data_end_ptr(const buffer * buf)
    {
            return buf->ptr + buf->end;
        }

#if (defined __cplusplus && !defined _WIN32)
}
#endif
#endif

