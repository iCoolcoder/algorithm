#ifndef LOG_H__
#define LOG_H__

#include <stdarg.h>
#include <stdio.h>
#include "port.h"

enum LogLevel
{
    LOG_LEVEL_TRC = 1,
    LOG_LEVEL_DBG = 2,
    LOG_LEVEL_INF = 3,
    LOG_LEVEL_WRN = 4,
    LOG_LEVEL_ERR = 5,
};

extern LogLevel log_level;

typedef void(LogCallback)(int level, const char *file, int line_num, const char *format, va_list args);

void DLLEXPORT LogSetCallback(LogCallback *cb);

void LogPrintf(int level, const char *file, int line_num, const char *format, ...);

void DLLEXPORT LogSetLevel(LogLevel lvl);
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

class LOGER {
public:
    LOGER() {}
    ~LOGER() {}
    static int open();
public:
    static FILE *fp;
};


#endif /* LOG_H__ */
