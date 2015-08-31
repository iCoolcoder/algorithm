/*
 * common macro defs
 * author: renzelong@youku.com
 * create: 2015
 *
 */

#ifndef COMMON_DEFS_
#define COMMON_DEFS_

#define DEF_FC_CAPABILITY_MAX            (500)
#define DEF_FC_CAPABILITY_DEFAULT        (500)
#define DEF_FC_RECEIVE_LEN_MAX           (64*1024*1024)
#define DEF_FC_RECEIVE_LEN_DEFAULT       (10*1024*1024)

#define FORWARD_MAX_SESSION_NUM_DEFAULT  (1000)


#define BUFFER_INIT_SIZE  (2 * 1024 * 1024)
#define BUFFER_MAX_SIZE  (4 * 1024 * 1024)
#define BUFFER_MAX_SIZE_V2  (10 * 1024 * 1024)

// used for interchange data buffer between forward server and forward client
#define FORWARD_INNER_BUFFER_SIZE (512*1024)
#define FORWARD_INNER_PROTO_BUFFER_SIZE (512)

#define FORWARD_DUMP_INTERVAL_TIME_LEN    (15) //seconds


#define FSS_SMALL_SESSION_SIZE_DEF FORWARD_INNER_BUFFER_SIZE
#define FSS_BIG_SESSION_SIZE_DEF (8 * 1024 * 1024)
#define FSS_BIG_SESSION_COUNT_DEF 20

// for rtp player and uploader
#define MAX_UDP_BUFFER_SIZE (64 * 1024)

#ifndef cache_t
#define cache_t media_manager::BackendCacheManagerInterface
#endif 

#ifndef cache_request_t
#define cache_request_t media_manager::cache_request_header
#endif 



#endif//COMMON_DEFS_
