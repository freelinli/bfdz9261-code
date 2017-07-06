#ifndef LOG_H
#define LOG_H

/**
 * ��־���
 */

#include "common.h"

/**
 * ��־����
 */
enum log_level {
	/** ���� */
	LOG_LEVEL_ERROR = 0,
	/** һ����Ϣ */
	LOG_LEVEL_INFO,
	/** ������Ϣ,������ʾ�ڲ�״̬ */
	LOG_LEVEL_DEBUG,
};

/**
 * ��¼��־.
 * @param levle log level, @see log_level
 * @param tag ��־��ǩ,��������ͬ��־������ģ��.
 * @param format @see printf
 * @return ִ�н��. 0:�ɹ�.
 */
int xh_log(enum log_level level, const char *tag, const char *format, ...);

#define LOG_ERROR(tag, foramt, ...) xh_log(LOG_LEVEL_ERROR, tag, "[%s] (%d) : " foramt, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_INFO(tag, foramt, ...) xh_log(LOG_LEVEL_INFO, tag, foramt, ##__VA_ARGS__)
#define LOG_DEBUG(tag,foramt, ...) xh_log(LOG_LEVEL_DEBUG, tag, foramt, ##__VA_ARGS__)

/** ������ն� */
#define LOG_DST_TERM	(0X01)
/** ������ļ� */
#define LOG_DST_FILE	(0X02)

/**
 * ��ֹ��־���.
 * @param dst, @see LOG_DST_TERM LOG_DST_FILE.
 * @return ִ�н��. 0:�ɹ�.
 */
int xh_log_dst_disable(int dst);

#endif // #ifndef LOG_H
