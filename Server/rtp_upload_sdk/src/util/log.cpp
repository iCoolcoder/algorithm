#include "log.h"
#include <stdio.h>
#include <time.h>

#ifdef WIN32
#define snprintf _snprintf
#endif

LogLevel log_level = LOG_LEVEL_ERR;

static LogCallback log_default, *log_cb = log_default;

#ifdef WIN32
struct tm *localtime_r(time_t* timer, struct tm* tp)
{
    struct tm* temp = localtime(timer);
    if (temp == NULL)
    {
        return NULL;
    }

    *tp = *temp;
    return tp;
}

#define sprintf _sprintf
#endif


static inline const char *log_level2str(int level)
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

void LogSetCallback(LogCallback *cbp)
{
    if (cbp == NULL)
    {
        return;
    }
    log_cb = cbp;
}

static void log_default(int level, const char *file, int line_num, const char *format, va_list args)
{
    time_t t;
    struct tm now_time;
    char buf[1024 * 8];

    if (NULL == format || level < log_level)
        return;

    t = time(NULL);
    if (localtime_r(&t, &now_time) == NULL) {
        return;
    }

    const char *log_type = log_level2str(level);

    int ret = snprintf(buf, sizeof(buf)-1, "%02d:%02d:%02d %s [%s:%d]",
        now_time.tm_hour, now_time.tm_min, now_time.tm_sec,
        log_type, file, line_num);

    if (ret > 0)
    {
        ret += vsnprintf(buf + ret, sizeof(buf)-ret - 1, format, args);
    }

    printf(buf);
    printf("\n");
}

void LogPrintf(int level, const char *file, int line_num, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_cb(level, file, line_num, format, args);
    va_end(args);

}

void LogSetLevel(LogLevel lvl)
{
    log_level = lvl;
}

LogLevel LogGetLevel()
{
    return log_level;
}

FILE *LOGER::fp = NULL;
int LOGER::open()
{
    fp = fopen("d:\\result.txt", "w+");
    return 0;
}
