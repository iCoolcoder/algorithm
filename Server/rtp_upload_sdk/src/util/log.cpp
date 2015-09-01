#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifdef _WIN32

#include "log.h"
#include <stdio.h>
#include <time.h>
#define snprintf _snprintf

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

static void log_default(int level, const char *file, int line_num, const char *format, ...)
{
    time_t t;
    struct tm now_time;
    char buf[1024 * 8];
    va_list args;

    if (NULL == format || level < log_level)
        return;

    t = time(NULL);
    if (localtime_r(&t, &now_time) == NULL) {
        return;
    }

    const char *log_type = log_level2str(level);

    int ret = snprintf(buf, sizeof(buf) -1, "%02d:%02d:%02d %s [%s:%d]",
        now_time.tm_hour, now_time.tm_min, now_time.tm_sec,
        log_type, file, line_num);

    if (ret > 0)
    {
        va_start(args, format);
        ret += vsnprintf(buf + ret, sizeof(buf)-ret - 1, format, args);
        va_end(args);
    }

    printf(buf);
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

#endif

#ifndef _WIN32
#include "log.h"
#include <assert.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include "statfile.h"

static log_config g_ctx;

static int
print_prefix(char *buf, size_t sz, struct tm *now_time,
int level, const char *file, int line_num)
{
	int ret;
	const char *log_type = log_level2str(level);

	ret = snprintf(buf, sz, "%02d:%02d:%02d %s [%s:%d]",
		now_time->tm_hour, now_time->tm_min, now_time->tm_sec,
		log_type, file, line_num);
	return ret;
}

static void
log_write(struct tm *now_time, int level, const char *file,
int line_num, const char *format, ...)
{
	char buf[1024 * 8];
	va_list ap;

	buf[sizeof(buf)-1] = '\0';
	int ret = print_prefix(buf, sizeof(buf)-1, now_time, level, file,
		line_num);

	if (ret > 0) {
		va_start(ap, format);
		ret += vsnprintf(buf + ret, sizeof(buf)-ret - 1, format, ap);
		va_end(ap);
	}

	if (ret > 0 && ret < (int)sizeof(buf)) {
		strcpy(buf + ret, "\n");
		ret += strlen("\n");
	}

	if (-1 != g_ctx.log_fd)
		ret = write(g_ctx.log_fd, buf, ret);
}

//ret -10~-20
static int
log_reopen_file(void)
{
	assert(LOG_MODE_FILE == g_ctx.mode);
	int fd = -1;
	time_t t = 0;
	struct tm curr_tm;
	char file_path[sizeof(g_ctx.file_path)];

	t = time(NULL);
	if (NULL == localtime_r(&t, &curr_tm))
		return -1;

	if (g_ctx.base_path[0] == '\0')
	{
		fprintf(stderr, "open log file error. base_path is NULL\n");
		return -1;
	}
	snprintf(file_path, sizeof(file_path)-1,
		"%s-%04d%02d%02d-%02d%02d%02d", g_ctx.base_path,
		curr_tm.tm_year + 1900, curr_tm.tm_mon + 1, curr_tm.tm_mday,
		curr_tm.tm_hour, curr_tm.tm_min, curr_tm.tm_sec);
	file_path[sizeof(file_path)-1] = '\0';

	fd = open(file_path, O_CREAT | O_APPEND | O_WRONLY, 0644);
	if (-1 == fd)
	{
		fprintf(stderr, "open log file error. errno: %d, base_path = %s, file_path = %s\n",
			errno, g_ctx.base_path, file_path);
		if (-1 != g_ctx.log_fd)
		{
			log_write(&curr_tm, LOG_LEVEL_DBG, __FILE__, __LINE__,
				"open log file error. errno: %d, base_path = %s, file_path = %s\n",
				errno, g_ctx.base_path, file_path);
		}
		return errno;
	}

	fcntl(fd, F_SETFD, FD_CLOEXEC);

	if (-1 != g_ctx.log_fd)
		close(g_ctx.log_fd);
	g_ctx.log_fd = fd;
	g_ctx.last_day = curr_tm;
	strncpy(g_ctx.file_path, file_path, sizeof(g_ctx.file_path));

	return 0;
}

static int
sig_safe_reopen()
{
	assert(LOG_MODE_FILE == g_ctx.mode);
	int ret;
	sigset_t this_set;
	sigset_t old_set;
	sigset_t *pold = NULL;

	sigfillset(&this_set);
	sigemptyset(&old_set);
	if (0 == sigprocmask(SIG_SETMASK, &this_set, &old_set))
		pold = &old_set;

	ret = log_reopen_file();
	if (NULL != pold)
		sigprocmask(SIG_SETMASK, pold, NULL);

	return ret;
}

int
log_init_file(const char *basepath, const char *name_prefix, int level,
size_t max_size)
{
	if (NULL == basepath
		|| strlen(basepath) >= BASE_PATH_LEN - 32 - strlen("-20130515-000000")
		|| strlen(name_prefix) > 32)
		return -1;

	char realbasepath[PATH_MAX];

	memset(realbasepath, 0, sizeof(realbasepath));
	memset(&g_ctx, 0, sizeof(g_ctx));
	g_ctx.mode = LOG_MODE_FILE;
	g_ctx.log_fd = -1;
	g_ctx.level = level;
	g_ctx.max_size = max_size;

	if (NULL == realpath(basepath, realbasepath)) {
		fprintf(stderr, "realpath failed. basepath = %s, error = %s\n",
			basepath, strerror(errno));
		return errno;
	}

	snprintf(g_ctx.base_path, sizeof(g_ctx.base_path) - 1,
		"%s/%s", realbasepath, name_prefix);
	return log_reopen_file();
}

int
log_init_net(const char *remotepath, const char *server_role,
const char *ip, uint16_t port, int level)
{
	memset(&g_ctx, 0, sizeof(g_ctx));
	g_ctx.mode = LOG_MODE_NET;
	g_ctx.level = level;
	g_ctx.log_fd = open_statfile_or_pipe(remotepath);
	if (g_ctx.log_fd < 0) {
		fprintf(stderr, "open_statfile_or_pipe failed. error = %s\n",
			strerror(errno));
		return -3;
	}
	snprintf(g_ctx.prefix_tags, sizeof(g_ctx.prefix_tags) - 1,
		"%s:%hu\tLOG\t%s", ip, port, server_role);

	return 0;
}

void
log_fini()
{
	if (-1 != g_ctx.log_fd)
		close(g_ctx.log_fd);
}

void
log_sig(const char *file, int line_num, int sig)
{
	int ret = 0;
	char buf[1024];
	time_t t;
	struct tm now_time;

	t = time(NULL);
	if (NULL == localtime_r(&t, &now_time))
		memset(&now_time, 0, sizeof(now_time));

	buf[sizeof(buf)-1] = '\0';
	if (LOG_MODE_FILE == g_ctx.mode) {
		ret = print_prefix(buf, sizeof(buf)-1, &now_time, LOG_LEVEL_INF, file,
			line_num);
	}
	else if (LOG_MODE_NET == g_ctx.mode) {
		ret =
			snprintf(buf, sizeof(buf)-1,
			"%s\t%04d-%02d-%02d %02d:%02d:%02d\t%s\t%s:%d\t",
			g_ctx.prefix_tags, now_time.tm_year + 1900,
			now_time.tm_mon + 1,
			now_time.tm_mday, now_time.tm_hour, now_time.tm_min,
			now_time.tm_sec, log_level2str(LOG_LEVEL_INF), file, line_num);
	}

	if (ret > 0) {
		ret +=
			snprintf(buf + ret, sizeof(buf)-ret - 1, "find sig %d(%s).\n",
			sig, strsignal(sig));
	}

	if (-1 != g_ctx.log_fd)
		ret = write(g_ctx.log_fd, buf, ret);
}

static void
check_reopen(struct tm *now_time)
{
	assert(LOG_MODE_FILE == g_ctx.mode);
	int ret = 0;
	struct stat buf;
	int s_boolean = FALSE;
	int t_boolean = now_time->tm_mday != g_ctx.last_day.tm_mday ||
		now_time->tm_mon != g_ctx.last_day.tm_mon ||
		now_time->tm_year != g_ctx.last_day.tm_year;
	if (!t_boolean) {
		ret = fstat(g_ctx.log_fd, &buf);
		if (0 != ret) {
			log_write(now_time, LOG_LEVEL_ERR, __FILE__, __LINE__,
				"fstat failed. error = %s", strerror(errno));
			return;
		}
		s_boolean = (int)(buf.st_size >= (long int)g_ctx.max_size);
	}
	if (t_boolean || s_boolean) {
		ret = sig_safe_reopen();
	}

	if (ret < 0) {
		log_write(now_time, LOG_LEVEL_ERR, __FILE__, __LINE__,
			"sig_safe_reopen failed. ret = %d", ret);
	}
	else if (ret > 0) {
		log_write(now_time, LOG_LEVEL_ERR, __FILE__, __LINE__,
			"sig_safe_reopen failed. error = %s", strerror(ret));
	}
}

void
log_print(int level, const char *file, int line_num, const char *format, ...)
{
	time_t t;
	struct tm now_time;
	char buf[1024 * 8];
	va_list ap;

	if (NULL == format || level < g_ctx.level)
		return;

	t = time(NULL);
	if (NULL == localtime_r(&t, &now_time)) {
		return;
	}
	int ret = 0;

	buf[sizeof(buf)-1] = '\0';
	if (LOG_MODE_FILE == g_ctx.mode) {
		check_reopen(&now_time);
		ret = print_prefix(buf, sizeof(buf)-1, &now_time, level, file,
			line_num);
	}
	else if (LOG_MODE_NET == g_ctx.mode) {
		ret =
			snprintf(buf, sizeof(buf)-1,
			"%s\t%04d-%02d-%02d %02d:%02d:%02d\t%s\t%s:%d\t",
			g_ctx.prefix_tags, now_time.tm_year + 1900,
			now_time.tm_mon + 1,
			now_time.tm_mday, now_time.tm_hour, now_time.tm_min,
			now_time.tm_sec, log_level2str(level), file, line_num);
	}

	if (ret > 0) {
		va_start(ap, format);
		ret += vsnprintf(buf + ret, sizeof(buf)-ret - 1, format, ap);
		va_end(ap);
	}

	if (LOG_MODE_FILE == g_ctx.mode) {
		if (ret > 0 && ret < (int)sizeof(buf)) {
			strcpy(buf + ret, "\n");
			ret += strlen("\n");
		}
	}
	else if (LOG_MODE_NET == g_ctx.mode) {
		if (ret > 0 && ret < (int)sizeof(buf)) {
			ret = ret > 1024 ? 1024 : ret;
			buf[ret++] = '\n';
		}
		else {
			return;
		}
	}

	if (-1 != g_ctx.log_fd)
		ret = write(g_ctx.log_fd, buf, ret);
}

void
log_set_level(int level)
{
	g_ctx.level = level;
}

void
log_set_max_size(size_t max_size)
{
	g_ctx.max_size = max_size;
}

int
log_get_stat(log_stat * state)
{
	memcpy(state, &g_ctx, sizeof(*state));
	return 0;
}

#endif


FILE *LOGER::fp = NULL;
int LOGER::open()
{
    fp = fopen("d:\\result.txt", "w+");
    return 0;
}
