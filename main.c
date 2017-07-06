
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "log.h"

#include "task_ttys1_zigbee_dev.h"
#include "task_ttys2_dtu_web.h"


#define  VERSION 107


static const char TAG[] = "Main";


void init_stack(void)
{
	size_t stacksize;
	pthread_attr_t thread_attr;
	
	pthread_attr_init(&thread_attr);
	
	pthread_attr_getstacksize(&thread_attr,&stacksize);
	LOG_DEBUG(TAG, "stack size(before set) : %d\n", stacksize);

	stacksize = 8 * 1024 * 1024;
	pthread_attr_setstacksize(&thread_attr,stacksize);

	pthread_attr_getstacksize(&thread_attr,&stacksize);
	LOG_DEBUG(TAG, "stack size(after set) : %d\n", stacksize);

	pthread_attr_destroy(&thread_attr);
}


int system_init(void)
{
	int ret = 0;

	//	if(xh_log_dst_disable(LOG_DST_FILE) == 0)\
	LOG_INFO(TAG, "disable LOG_DST_FILE\n");

	create_sqlite3();
	get_sqlite3_serial_max();

	
exit:
	return ret;
}

int main(int argc, char* argv[])
{
	int ret = 0;
	ret = system_init();
	LOG_INFO(TAG, "main is start \r\n");
	LOG_INFO(TAG, "VERSION = %d\r\n", VERSION);
	init_stack();

	if (ret < 0) {
		ret = -1;
		LOG_ERROR(TAG, "system_init error, exit...\n");
		goto exit;
	}


	Task_TTYS2_Dtu_Web();
	sleep(1);
//	Task_TTYS1_Zigbee_Dev();

	Task_launcher();
	while(1)
	{
		send_sqlite_data_to_web();
		auto_control_dev();
		//	LOG_INFO(TAG, "main is running \r\n");
		sleep(3);
	}
exit:
	return ret;

}

