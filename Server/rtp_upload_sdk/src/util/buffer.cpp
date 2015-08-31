/*****************************************************
* YueHonghui, 2013-06-23
* hhyue@tudou.com
* copyright:youku.com
* ported from QueHongyu
* **************************************************/

#include "buffer.h"
#include <errno.h>
#include <assert.h>
#include <string.h>
#include "memory.h"
#include "log.h"
#ifndef _WIN32
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#endif

buffer *
buffer_init(size_t size)
{
    buffer *b;

    b = (buffer *)mcalloc(1, sizeof(*b));
    if (b) {
        b->start = 0;
        b->end = 0;
        b->size = size;
        b->max = size;
#define BUFFER_PIECE_SIZE 64
        b->size += (BUFFER_PIECE_SIZE - 1) - ((b->size + BUFFER_PIECE_SIZE - 1) % BUFFER_PIECE_SIZE);
#undef BUFFER_PIECE_SIZE
        b->ptr = (char *)mcalloc(1, b->size);
        if (b->ptr == NULL) {
            mfree(b);
            b = NULL;
        }
    }
    return b;
}

buffer *
buffer_create_max(size_t size, size_t max)
{
    if (size > max)
        return NULL;
    buffer *b = buffer_init(size);

    if (b)
    {
        b->max = max;
        assert((b->size <= b->max));
        if (b->size > b->max)
        {
            ERR("buffer_create_max error! max_size=%ld, size=%ld.", b->max, b->size);
        }
    }
    return b;
}

void buffer_free(buffer * b)
{
    if (b) {
        mfree(b->ptr);
        mfree(b);
    }
}

int buffer_expand(buffer * b, size_t size)
{
    assert(NULL != b);
    char *p;

    if (size >= b->max)
    {
        //        assert(0);
        WRN("buffer_expand error! size=%ld, max_size=%ld.", size, b->max);
        return BUFFER_CAPACITY_ERROR;
    }
    if (b->size < size) {
#define BUFFER_PIECE_SIZE 64
        b->size += (BUFFER_PIECE_SIZE - 1) - ((b->size + BUFFER_PIECE_SIZE - 1) % BUFFER_PIECE_SIZE);
#undef BUFFER_PIECE_SIZE
        p = (char *)mrealloc(b->ptr, size);
        if (p) {
            b->ptr = p;
            b->size = size;
            return 0;
        }
        else
        {
            //            assert(0);
            WRN("buffer_expand error! b->size=%ld, max_size=%ld.", b->size, b->max);
            return BUFFER_MALLOC_ERROR;
        }
    }
    return 0;
}

int buffer_expand_capacity(buffer * buf, size_t cap_sz)
{
    assert(NULL != buf);
    assert(NULL != buf->ptr);
    int ret = 0;

    if (buf->size - buf->end >= cap_sz)
        return 0;

    if (buffer_data_len(buf) + cap_sz >= buf->max)
    {
        //        assert(0);
        WRN("buffer_expand_capacity error! buffer_data_len=%ld, cap_sz=%ld buf->max=%ld.",
            buffer_data_len(buf), cap_sz, buf->max);
        return BUFFER_CAPACITY_ERROR;
    }

    if (buffer_data_len(buf) + cap_sz <= buf->size) {
        buffer_adjust(buf);
        return 0;
    }

    ret = buffer_expand(buf, buffer_data_len(buf) + cap_sz);
    if (0 != ret)
    {
        //        assert(0);
        WRN("buffer_expand_capacity error! buffer_data_len=%ld, cap_sz=%ld buf->max=%ld.",
            buffer_data_len(buf), cap_sz, buf->max);
        return ret;
    }

    buffer_adjust(buf);
    return 0;
}

int buffer_append(buffer * dst, buffer * src)
{
    assert(NULL != dst);
    assert(NULL != src);
    assert(NULL != dst->ptr);
    assert(NULL != src->ptr);
    return buffer_append_ptr(dst, src->ptr + src->start, src->end - src->start);
}

int buffer_append_ptr(buffer * dst, const void *src, size_t src_len)
{
    assert(NULL != dst);
    assert(NULL != src);

    if (dst->end - dst->start + src_len > dst->size)
        return BUFFER_OVER_SIZE;

    if (dst->size - dst->end < src_len)
        buffer_adjust(dst);

    memmove(dst->ptr + dst->end, src, src_len);
    dst->end += src_len;
    return 0;
}

int buffer_append_byte(buffer * dst, unsigned char byte)
{
    return buffer_append_ptr(dst, &byte, 1);
}

int buffer_copy(buffer * dst, buffer * src)
{
    assert(NULL != dst);
    assert(NULL != src);
    assert(NULL != dst->ptr);
    assert(NULL != src->ptr);
    if (src->end - src->start == 0)
        return 0;

    if (src->end - src->start > dst->size)
        return BUFFER_OVER_SIZE;

    memcpy(dst->ptr, src->ptr + src->start, src->end - src->start);
    dst->end = src->end - src->start;
    return 0;
}

size_t buffer_eat(buffer * buf, size_t bytes)
{
    assert(NULL != buf);

    size_t len = bytes < (buf->end - buf->start) ? bytes : (buf->end
        - buf->start);
    buf->start += len;

    return len;
}

size_t buffer_ignore(buffer * buf, size_t bytes)
{
    assert(NULL != buf);

    if (bytes == 0)
        return bytes;

    if (bytes >= buf->end - buf->start) {
        size_t ret = buf->end - buf->start;

        buf->start = buf->end = 0;
        return ret;
    }

    memmove(buf->ptr, buf->ptr + buf->start + bytes, buf->end - buf->start
        - bytes);
    buf->end -= (buf->start + bytes);
    buf->start = 0;
    return bytes;
}

//int buffer_read_fd(buffer * b, int fd)
//{
//    return buffer_read_fd_max(b, fd, 1024 * 1024 * 1024);
//}
//
//int buffer_read_fd_max(buffer * b, int fd, size_t max)
//{
//    int toread = 0, r;
//
//    if (fd <= 0 || b == NULL) {
//        ERR("error fd or buffer null");
//        return BUFFER_SOCKET_ERROR;
//    }
//    if (ioctl(fd, FIONREAD, &toread)) {
//        if (errno != EAGAIN && errno != EINTR) {
//            ERR("ioctl error");
//            return BUFFER_SOCKET_ERROR;
//        }
//        else
//            return 0;
//    }
//
//    /* try to detect whether it's eof */
//    if (toread == 0)
//        toread = 1;
//    toread = toread <= (int)max ? toread : (int)max;
//    if (b->max > 0)
//        toread = toread <= (int)b->max - buffer_data_len(b) ? toread : (int)b->max
//        - buffer_data_len(b);
//
//    if (toread >= (int)(b->size - b->end))
//    {
//        int ret = buffer_expand_capacity(b, toread);
//        if (0 != ret)
//        {
//            ERR("buffer_expand_capacity error");
//            return ret; /* expand receive buffer */
//        }
//    }
//
//    toread = toread < (int)buffer_capacity(b) ? toread : (int)buffer_capacity(b);
//
//    if (toread == 0)
//    {
//        WRN("will read 0 byte from socket!!");
//        return 0;
//    }
//    r = read(fd, b->ptr + b->end, toread);
//    if ((r == -1 && (errno != EINTR && errno != EAGAIN)) || r == 0)
//    {
//        /* connection closed or reset */
//        if (r == 0)
//        {
//            WRN("read 0 bytes");
//        }
//        return BUFFER_SOCKET_ERROR;
//    }
//    if (r > 0)
//    {
//        b->end += r;
//        return r;
//    }
//    else {
//        return 0;
//    }
//}

//int buffer_write_fd(buffer * b, int fd)
//{
//    return buffer_write_fd_max(b, fd, 1024 * 1024);
//}
//
//int buffer_write_fd_max(buffer * b, int fd, size_t max)
//{
//    int ret = 0, total = 0;
//    size_t end = b->start + max > b->end ? b->end : b->start + max;
//
//    while (end - b->start > 0) {
//        ret = write(fd, b->ptr + b->start, end - b->start);
//        if (ret < 0) {
//            if (EAGAIN == errno || EWOULDBLOCK == errno)
//                break;
//            if (EINTR != errno)
//                return BUFFER_SOCKET_ERROR;
//        }
//        else if (0 == ret) {
//            break;
//        }
//        else {
//            total += ret;
//            b->start += ret;
//        }
//    }
//    return total;
//}
//
//int 
//buffer_udp_recvfrom_fd(buffer *b, int fd, struct sockaddr_in &client_addr)
//{
//    return buffer_udp_recvfrom_fd_max(b, fd, buffer_capacity(b), client_addr);
//}
//
//int 
//buffer_udp_recvfrom_fd_max(buffer *b, int fd, size_t max, struct sockaddr_in &client_addr)
//{
//    if (fd <= 0 || b == NULL) {
//        ERR("error fd or buffer null");
//        return BUFFER_SOCKET_ERROR;
//    }
//    if (max > buffer_capacity(b)) {
//        return BUFFER_CAPACITY_ERROR;
//    }
//
//    socklen_t size = sizeof(struct sockaddr);
//    int r = recvfrom(fd, b->ptr+b->end, buffer_capacity(b), 0, (struct sockaddr *)&client_addr, &size);
//    if ((r == -1 && (errno != EINTR && errno != EAGAIN)) || r == 0)
//    {
//        /* connection closed or reset */
//        if (r == 0)
//        {
//            WRN("read 0 bytes");
//        }
//        return BUFFER_SOCKET_ERROR;
//    }
//    if (r > 0)
//    {
//        b->end += r;
//        return r;
//    }
//    else {
//        return 0;
//    }
//}
