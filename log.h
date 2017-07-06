#ifndef LOG_H
#define LOG_H

/**
 * 日志输出
 */

#include "common.h"

/**
 * 日志级别
 */
enum log_level {
	/** 错误 */
	LOG_LEVEL_ERROR = 0,
	/** 一般信息 */
	LOG_LEVEL_INFO,
	/** 调试信息,用于显示内部状态 */
	LOG_LEVEL_DEBUG,
};

/**
 * 记录日志.
 * @param levle log level, @see log_level
 * @param tag 日志标签,用于区别不同日志所属的模块.
 * @param format @see printf
 * @return 执行结果. 0:成功.
 */
int xh_log(enum log_level level, const char *tag, const char *format, ...);

#define LOG_ERROR(tag, foramt, ...) xh_log(LOG_LEVEL_ERROR, tag, "[%s] (%d) : " foramt, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_INFO(tag, foramt, ...) xh_log(LOG_LEVEL_INFO, tag, foramt, ##__VA_ARGS__)
#define LOG_DEBUG(tag,foramt, ...) xh_log(LOG_LEVEL_DEBUG, tag, foramt, ##__VA_ARGS__)

/** 输出到终端 */
#define LOG_DST_TERM	(0X01)
/** 输出到文件 */
#define LOG_DST_FILE	(0X02)

/**
 * 禁止日志输出.
 * @param dst, @see LOG_DST_TERM LOG_DST_FILE.
 * @return 执行结果. 0:成功.
 */
int xh_log_dst_disable(int dst);

#endif // #ifndef LOG_H
