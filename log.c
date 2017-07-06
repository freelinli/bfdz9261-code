#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#include <sys/stat.h>
#include <sys/mman.h>
#include <libgen.h>

#include "log.h"

#define LOG_FILE_SIZE_LIMIT	(2 * 1024 * 1024)

static int 			log_dst = LOG_DST_TERM | LOG_DST_FILE;
static const char  	*log_file_dir = "./";
pthread_mutex_t 	log_mutex = PTHREAD_MUTEX_INITIALIZER;

static int get_time_stamp(char *buf, int length)
{
	time_t time_utc;
	struct tm time_local;

	time_utc = time(NULL);
	localtime_r(&time_utc, &time_local);
	strftime(buf, length, "%m-%d %H:%M:%S", &time_local);
}

static int log_to_terminal(enum log_level level, const char *tag, const char *format, va_list ap)
{
	char time_stamp[32];

	get_time_stamp(time_stamp, sizeof(time_stamp));
	
	if (level == LOG_LEVEL_ERROR) {
		printf("[E %s] <%s> : ", time_stamp, tag);
		vprintf(format, ap);
	} else if (level == LOG_LEVEL_INFO) {
		printf("[I %s] <%s> : ", time_stamp, tag);
		vprintf(format, ap);
	} else if (level == LOG_LEVEL_DEBUG) {
		printf("[D %s] <%s> : ", time_stamp, tag);
		vprintf(format, ap);
	}
	
	return 0;
}

static FILE *get_fp(void)
{
	static FILE *fp = NULL;
	char buf[256];
	char time_stamp[32];

label_openfile:
	if (fp == NULL) {
		strcpy(buf, log_file_dir);
		strcat(buf, "/log.txt");
		fp = fopen(buf, "a+");
		if (!fp) {
			printf("LOG : cat not open file : %s\n", buf);
			return NULL;
		}
	}
	
	if (ftell(fp) >= LOG_FILE_SIZE_LIMIT) {
		fclose(fp);
		get_time_stamp(time_stamp, sizeof(time_stamp));
		strcpy(buf, "mv ");
		strcat(buf, log_file_dir);
		strcat(buf, "/log.txt ");
		strcat(buf, log_file_dir);
		strcat(buf, "/log-old.txt");
		system(buf);

		fp = NULL;
		goto label_openfile;
	} else {
		return fp;
	}
}

static int log_to_file(enum log_level level, const char *tag, const char *format, va_list ap)
{
	char time_stamp[32];
	FILE *fp;
	
	fp = get_fp();
	if (fp == NULL) {
		return -1;
	}
	get_time_stamp(time_stamp, sizeof(time_stamp));
	
	if (level == LOG_LEVEL_ERROR) {
		fprintf(fp, "[E %s] <%s> : ", time_stamp, tag);
		vfprintf(fp, format, ap);
	} else if (level == LOG_LEVEL_INFO) {
		fprintf(fp, "[I %s] <%s> : ", time_stamp, tag);
		vfprintf(fp, format, ap);
	} else if (level == LOG_LEVEL_DEBUG) {
		fprintf(fp, "[D %s] <%s> : ", time_stamp, tag);
		vfprintf(fp, format, ap);
	}
	return 0;
}

int xh_log(enum log_level level, const char *tag, const char *format, ...)
{
	va_list ap;
 	pthread_mutex_lock(&log_mutex);
	va_start(ap, format);
	if (log_dst & LOG_DST_TERM) {
		log_to_terminal(level, tag, format, ap);
	}
	if (log_dst & LOG_DST_FILE) {
		log_to_file(level, tag, format, ap);
	}
	va_end(ap);
	pthread_mutex_unlock(&log_mutex);
	return 0;
}

int xh_log_dst_disable(int dest)
{
	log_dst &= (~dest);
	return 0;
}

