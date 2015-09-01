#ifndef LOG_H__
#define LOG_H__

#include <stddef.h>
#ifndef _WIN32
#include <limits.h>
#endif
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#include <stdio.h>
#ifdef _WIN32
#define PATH_MAX        4096        /* # chars in a path name including nul */
#endif

#ifdef _WIN32
#include <stdarg.h>
#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif
enum LogLevel
{
    LOG_LEVEL_TRC = 1,
    LOG_LEVEL_DBG = 2,
    LOG_LEVEL_INF = 3,
    LOG_LEVEL_WRN = 4,
    LOG_LEVEL_ERR = 5,
};

extern LogLevel log_level;

typedef void(LogCallback)(int level, const char *file, int line_num, const char *format, ...);

void DLLEXPORT LogSetCallback(LogCallback *cb);

void LogPrintf(int level, const char *file, int line_num, const char *format, ...);

void LogSetLevel(LogLevel lvl);
LogLevel LogGetLevel();

#define TRC(...) do { \
if (log_level > LOG_LEVEL_TRC) break; \
    LogPrintf(LOG_LEVEL_TRC, __FILE__, __LINE__, __VA_ARGS__);	\
} while (0)

#define DBG(...) do { \
if (log_level > LOG_LEVEL_DBG) break; \
    LogPrintf(LOG_LEVEL_DBG, __FILE__, __LINE__, __VA_ARGS__);	\
} while (0)

#define INF(...) do { \
if (log_level > LOG_LEVEL_INF) break; \
    LogPrintf(LOG_LEVEL_INF, __FILE__, __LINE__, __VA_ARGS__);	\
} while (0)

#define WRN(...) do { \
if (log_level > LOG_LEVEL_WRN) break; \
    LogPrintf(LOG_LEVEL_WRN, __FILE__, __LINE__, __VA_ARGS__);	\
} while (0)

#define ERR(...) do { \
if (log_level > LOG_LEVEL_ERR) break; \
    LogPrintf(LOG_LEVEL_ERR, __FILE__, __LINE__, __VA_ARGS__);	\
} while (0)

#endif

#ifndef _WIN32
#if (defined __cplusplus && !defined _WIN32)
extern "C"
{
#endif

#define BASE_PATH_LEN (PATH_MAX)

enum
{
    LOG_MODE_NET = 1,
    LOG_MODE_FILE = 2,
};

typedef struct log_config
{
    int log_fd;
    int mode;
    int level;
    size_t max_size;
    struct tm last_day;
    char base_path[BASE_PATH_LEN];
    char file_path[BASE_PATH_LEN];
    char prefix_tags[1024];
} log_config;


enum
{
	LOG_LEVEL_TRC = 1,
	LOG_LEVEL_DBG = 2,
	LOG_LEVEL_INF = 3,
	LOG_LEVEL_WRN = 4,
	LOG_LEVEL_ERR = 5,
};


typedef log_config log_stat;

//strlen(name_prefix) should <= 32
//example:
//   current_dir = /home/test/server
//   basepath = "logs" or "/home/test/server/logs"
//   name_prefix = "receiver"
//   then the log file of 20130515 will at /home/test/server/logs/receiver-20130515
int log_init_file(const char *basepath, const char *name_prefix, int level,
                  size_t max_size);
int log_init_net(const char *remotepath, const char *server_role,
                 const char *ip, uint16_t port, int level);

static inline int
log_str2level(const char *level)
{
    if(0 == strncmp("TRC", level, 3))
        return LOG_LEVEL_TRC;
    if(0 == strncmp("DBG", level, 3))
        return LOG_LEVEL_DBG;
    if(0 == strncmp("INF", level, 3))
        return LOG_LEVEL_INF;
    if(0 == strncmp("WRN", level, 3))
        return LOG_LEVEL_WRN;
    if(0 == strncmp("ERR", level, 3))
        return LOG_LEVEL_ERR;
    return -1;
}

static inline const char *
log_level2str(int level)
{
    switch (level) {
    case LOG_LEVEL_TRC:
        return "TRC";
    case LOG_LEVEL_DBG:
        return "DBG";
    case LOG_LEVEL_INF:
        return "INF";
    case LOG_LEVEL_WRN:
        return "WRN";
    case LOG_LEVEL_ERR:
        return "ERR";
    default:
        break;
    }
    return "UNDEFINED";
}

void log_fini();
void
log_print(int level, const char *file, int line_num, const char *format, ...)
ATTR_PRINTF(4, 5);

     void log_sig(const char *file, int line_num, int sig);

     void log_set_level(int level);

     void log_set_max_size(size_t max_size);

     int log_get_stat(log_stat * state);

#define TRC(...) do { \
    log_print(LOG_LEVEL_TRC, __FILE__, __LINE__, __VA_ARGS__);	\
     } while (0)

#define DBG(...) do { \
    log_print(LOG_LEVEL_DBG, __FILE__, __LINE__, __VA_ARGS__);	\
     } while (0)

#define INF(...) do {\
    log_print(LOG_LEVEL_INF, __FILE__, __LINE__, __VA_ARGS__);	\
     } while (0)

#define WRN(...) do {\
    log_print(LOG_LEVEL_WRN, __FILE__, __LINE__, __VA_ARGS__);	\
     } while (0)

#define ERR(...) do {\
    log_print(LOG_LEVEL_ERR, __FILE__, __LINE__, __VA_ARGS__);	\
     } while (0)

#define SIG(sig) do {					\
    log_sig(__FILE__, __LINE__, sig);	\
     } while (0)

#ifdef DEBUG
#define ASSERTR(expr, ret) { \
     if (!(expr)) \
     { \
     log_print(LOG_LEVEL_ERR, __FILE__, __LINE__, "ASSERT: " #expr); \
     assert(false); \
     } \
     }
#define ASSERTV(expr) { \
     if (!(expr)) \
     { \
     log_print(LOG_LEVEL_ERR, __FILE__, __LINE__, "ASSERT: " #expr); \
     assert(false); \
     } \
     }
#define ASSERTE(expr) ((expr) ? true : (log_print(LOG_LEVEL_ERR, __FILE__, __LINE__, "ASSERT: " #expr), assert(false), false))
#define ASSERTSR(expr, fail_statement, ret) { \
     if (!(expr)) \
     { \
     log_print(LOG_LEVEL_ERR, __FILE__, __LINE__, "ASSERT: " #expr); \
     { fail_statement; } \
     assert(false); \
     return (ret); \
     } \
     }
#define ASSERTSV(expr, fail_statement) { \
     if (!(expr)) \
     { \
     log_print(LOG_LEVEL_ERR, __FILE__, __LINE__, "ASSERT: " #expr); \
     { fail_statement; } \
     assert(false); \
     return; \
     } \
     }
#else
#define ASSERTR(expr, ret) { \
     if (!(expr)) \
     { \
     log_print(LOG_LEVEL_ERR, __FILE__, __LINE__, "ASSERT: " #expr); \
     return (ret); \
     } \
     }
#define ASSERTV(expr) { \
     if (!(expr)) \
     { \
     log_print(LOG_LEVEL_ERR, __FILE__, __LINE__, "ASSERT: " #expr); \
     return; \
     } \
     }
#define ASSERTE(expr) ((expr) ? true : (log_print(LOG_LEVEL_ERR, __FILE__, __LINE__, "ASSERT: " #expr), false))
#define ASSERTSR(expr, fail_statement, ret) { \
     if (!(expr)) \
     { \
     log_print(LOG_LEVEL_ERR, __FILE__, __LINE__, "ASSERT: " #expr); \
     { fail_statement; } \
     return (ret); \
     } \
     }
#define ASSERTSV(expr, fail_statement) { \
     if (!(expr)) \
     { \
     log_print(LOG_LEVEL_ERR, __FILE__, __LINE__, "ASSERT: " #expr); \
     { fail_statement; } \
     return; \
     } \
     }
#endif // DEBUG

#ifdef DEBUG
#define ASSERTR(expr, ret) { \
     if (!(expr)) \
     { \
     log_print(LOG_LEVEL_ERR, __FILE__, __LINE__, "ASSERT: " #expr); \
     assert(false); \
     } \
     }
#define ASSERTV(expr) { \
     if (!(expr)) \
     { \
     log_print(LOG_LEVEL_ERR, __FILE__, __LINE__, "ASSERT: " #expr); \
     assert(false); \
     } \
     }
#else
#define ASSERTR(expr, ret) { \
     if (!(expr)) \
     { \
     log_print(LOG_LEVEL_ERR, __FILE__, __LINE__, "ASSERT: " #expr); \
     return (ret); \
     } \
     }
#define ASSERTV(expr) { \
     if (!(expr)) \
     { \
     log_print(LOG_LEVEL_ERR, __FILE__, __LINE__, "ASSERT: " #expr); \
     return; \
     } \
     }
#endif // DEBUG

#ifdef DEBUG
#define ASSERTR(expr, ret) { \
     if (!(expr)) \
     { \
     log_print(LOG_LEVEL_ERR, __FILE__, __LINE__, "ASSERT: " #expr); \
     assert(false); \
     } \
     }
#define ASSERTV(expr) { \
     if (!(expr)) \
     { \
     log_print(LOG_LEVEL_ERR, __FILE__, __LINE__, "ASSERT: " #expr); \
     assert(false); \
     } \
     }
#else
#define ASSERTR(expr, ret) { \
     if (!(expr)) \
     { \
     log_print(LOG_LEVEL_ERR, __FILE__, __LINE__, "ASSERT: " #expr); \
     return (ret); \
     } \
     }
#define ASSERTV(expr) { \
     if (!(expr)) \
     { \
     log_print(LOG_LEVEL_ERR, __FILE__, __LINE__, "ASSERT: " #expr); \
     return; \
     } \
     }
#endif // DEBUG

#if (defined __cplusplus && !defined _WIN32)
}
#endif

#endif

class LOGER {
public:
    LOGER(){}
    ~LOGER() {}
    static int open();
public:
    static FILE *fp;
};



#endif // LOG_H__
