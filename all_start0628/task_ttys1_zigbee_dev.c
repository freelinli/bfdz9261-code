#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>
#include<string.h>
#include "log.h"
#include "crc16.h"
#include "tty.h"
#include <pthread.h>


#include "json_function.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include "task_ttys1_zigbee_dev.h"


#define CONTROLLER
#define COLLECTOR
#define TRANSFORMER
#define AUTO_SEND_TO_WEB

static const char TAG[] = "Task_TTYS1_Zigbee_Dev";
unsigned char Buf_Recv[512];

int fd, nset;

enum
{
	BUF_COLLECTOR_TYPE =0,
	BUF_TRANSFORMER_TYPE,
	BUF_CONTROLLER_TYPE
};

//                              0xa5, 0x5a, 帧头 0x01 大棚号   设备类型  数据长度  数据体     + crc16校验两位 + bf (帧尾)
//                                                            	03 采集器
//																05 控制器
//																02 采集互感器
//unsigned char Buf_Collector_Send[32] 	= 	{0xa5, 0x5a, 0x01, 0x03, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x0, 0x0, 0x00};
//unsigned char Buf_Transformer_Send[32] 	= 	{0xa5, 0x5a, 0x01, 0x02, 0x01, 0x00, 0x0, 0x0, 0x00};
//unsigned char Buf_Controller_Send[32]  	= 	{0xa5, 0x5a, 0x01, 0x05};
unsigned char Buf_Send[32] = {0};

int Send_Function(int fd, int id, unsigned char Buf_Send_Type, unsigned char buf[], int len);

#define SLEEP_TIME_1 3

extern unsigned char flag_lost_connect_to_web; 

#define MSEC_PER_SEC		1000L
#define USEC_PER_SEC		1000000L
#define NSEC_PER_SEC		1000000000L
#define NSEC_PER_USEC		1000L
// 获取时间差值 # ms
int get_time_val_diff(struct timeval *time_begin, struct timeval *time_end)
{
	int sec;
	int usec;
	sec = time_end->tv_sec - time_begin->tv_sec;
	usec = time_end->tv_usec - time_begin->tv_usec;
	return (sec * MSEC_PER_SEC + usec / (USEC_PER_SEC / MSEC_PER_SEC));
}


static void *thread_Task_TTYS1_Zigbee_Dev(void *data)
{
	int  i = 0, j = 0,temp = 0, id;
	int flag_3s_count = 0, flag_30s_count = 0;
	unsigned char temp_buf[16] = {0x00};
	struct timeval time_begin;
	struct timeval time_end;
	int time_elapsed = 0,time_elapsed_last;

	LOG_DEBUG(TAG, "Task_TTYS1_Zigbee_Dev  start\n");

	LOG_DEBUG(TAG, "%s start \r\n", Task_TTYS1_DEV );
	LOG_DEBUG(TAG, "%s start gettid = %u   %u \r\n", Task_TTYS1_DEV,  pthread_self(), syscall(SYS_gettid));

	cjson_from_config_to_sharemem();

	LOG_DEBUG(TAG, " cjson_from_config_to_sharemem ok\r\n");



	fd = open(Task_TTYS1_DEV, O_RDWR| O_NONBLOCK);//打开串口
	//	fd = open(Task_TTYS1_DEV, O_RDWR);//打开串口

	if (fd == -1)
	{
		LOG_ERROR(TAG,"open %s FAIL\r\n", Task_TTYS1_DEV);
		pthread_exit(NULL);
	}

	LOG_DEBUG(TAG,"open ok\r\n");
	nset = set_opt(fd,9600, 8, 'N', 1);//设置串口属性
	if (nset == -1)
	{
		LOG_ERROR(TAG, "set %s  FAIL\r\n",Task_TTYS1_DEV);
		pthread_exit(NULL);
	}
	LOG_DEBUG(TAG, "set %s  ok\r\n",Task_TTYS1_DEV);

	gettimeofday(&time_begin, NULL);

	while(1)
	{

#ifdef CONTROLLER
		{
			bzero(temp_buf,sizeof(temp_buf));

			while(Get_Controller_Json_Data(temp_buf, &temp, &id) == 1)
			{
				Send_Function(fd, id, BUF_CONTROLLER_TYPE,temp_buf, temp);
				sleep(SLEEP_TIME_1);
			}
		}
#endif
		control_dev_by_time(fd);
		//auto_control_dev();

		if(flag_3s_count < 40)
		{
			usleep(1000*100);
			gettimeofday(&time_end, NULL);	
			time_elapsed  = get_time_val_diff(&time_begin, &time_end); //ms

			time_elapsed  = time_elapsed / 1000;  // s
			if(time_elapsed_last != time_elapsed)
			{
				flag_3s_count = time_elapsed / 3;
				LOG_DEBUG(TAG, "(%d)s, flag_3s_count = %d\r\n",time_elapsed, flag_3s_count);
				time_elapsed_last = time_elapsed;


#ifdef COLLECTOR
			bzero(temp_buf,sizeof(temp_buf));

			while(Get_Collecter_Json_Data(temp_buf, &temp, &id) == 1)
			{
				//	char i = -1 ; &&(i == 255) return ; in at91sam9261
				Send_Function(fd,id, BUF_COLLECTOR_TYPE,temp_buf, temp);
				sleep(SLEEP_TIME_1);
			}
#endif

#ifdef TRANSFORMER

	
			i = 0;
			bzero(temp_buf,sizeof(temp_buf));

			if(Get_Transformer_Json_Data(temp_buf, &temp))
			{
				LOG_DEBUG(TAG, "TRANSFORMER len = %d\r\n", temp);
				for(i = 0; i < temp; i ++)	
				{
					LOG_DEBUG(TAG, "TRANSFORMER id  = %d\r\n", temp_buf[i]);
					Send_Function(fd,temp_buf[i],BUF_TRANSFORMER_TYPE, &(temp_buf[i]), 1);
					sleep(SLEEP_TIME_1);
				}
			}
#endif

			}
		}

		if(flag_3s_count >= 2)
		{
			gettimeofday(&time_begin, NULL);
			flag_3s_count = 0;
#ifdef AUTO_SEND_TO_WEB
			LOG_DEBUG(TAG, "send_data_to_web_on_time , just go\r\n");
			send_data_to_web_on_time();
//			if(flag_lost_connect_to_web == 1)
//			save_data_to_sqlite();
#endif
		}

	}
	close(fd);
	return 0;
}



int Get_Collecter_id(cJSON *jsonroot, int a6, int glsen, int *max, int *min)
{
	cJSON *JsonOption2, *JsonNode3, *JsonData4;
	int i, j, k;
	*max = *min = 0;
	if(jsonroot == NULL)
	{
		LOG_ERROR(TAG, "Get_Collecter_id error jsonroot is null\r\n");
	}else{
		JsonOption2 = cJSON_GetObjectItem(jsonroot, "collector");
		if(JsonOption2 == NULL)
		{
			LOG_ERROR(TAG, "Get_Collecter_id  collector  is null\r\n");
		}else{
			for(i = 0; i < cJSON_GetArraySize(JsonOption2); i++)
			{
				JsonNode3 = cJSON_GetArrayItem(JsonOption2, i);
				if(JsonNode3 == NULL)
				{	
					LOG_ERROR(TAG, "Get_Collecter_id  collector i  is null\r\n");
				}else{
					JsonData4 = cJSON_GetObjectItem(JsonNode3, "a6");
					if(JsonData4 == NULL)
					{	
						LOG_ERROR(TAG, "Get_Collecter_id  collector a6  is null\r\n");
					}else{

						for(j = 0; j < cJSON_GetArraySize(JsonData4); j++)
						{
							if(cJSON_GetArrayItem(JsonData4, j)->valueint == a6)
								if(cJSON_GetObjectItem(JsonNode3, "a2") != NULL)
								{

									LOG_DEBUG(TAG, "Get_Collecter_id  collector switch glsen = %d\r\n",glsen );
									switch(glsen)
									{
									case 1:
										if(cJSON_GetObjectItem(JsonNode3, "temps") != NULL)
											*max = cJSON_GetObjectItem(JsonNode3, "temps")->valueint; 
										if(cJSON_GetObjectItem(JsonNode3, "tempx") != NULL)
											*min = cJSON_GetObjectItem(JsonNode3, "tempx")->valueint; 
										break;
									case 2:
										if(cJSON_GetObjectItem(JsonNode3, "humiditys") != NULL)
											*max = cJSON_GetObjectItem(JsonNode3, "humiditys")->valueint; 
										if(cJSON_GetObjectItem(JsonNode3, "humidityx") != NULL)
											*min = cJSON_GetObjectItem(JsonNode3, "humidityx")->valueint; 
										break;
									case 3:
										if(cJSON_GetObjectItem(JsonNode3, "illumins") != NULL)
											*max = cJSON_GetObjectItem(JsonNode3, "illumins")->valueint; 
										if(cJSON_GetObjectItem(JsonNode3, "illuminx") != NULL)
											*min = cJSON_GetObjectItem(JsonNode3, "illuminx")->valueint; 
										break;
									case 4:
										if(cJSON_GetObjectItem(JsonNode3, "co2s") != NULL)
											*max = cJSON_GetObjectItem(JsonNode3, "co2s")->valueint; 
										if(cJSON_GetObjectItem(JsonNode3, "co2x") != NULL)
											*min = cJSON_GetObjectItem(JsonNode3, "co2x")->valueint; 
										break;
									case 5:
										if(cJSON_GetObjectItem(JsonNode3, "oilHumiditys") != NULL)
											*max = cJSON_GetObjectItem(JsonNode3, "oilHumiditys")->valueint; 
										if(cJSON_GetObjectItem(JsonNode3, "oilHumidityx") != NULL)
											*min = cJSON_GetObjectItem(JsonNode3, "oilHumidityx")->valueint; 
										break;
									default:
										LOG_DEBUG(TAG, "Get_Collecter_id  no this menxian\r\n");
										break;
									}

									LOG_DEBUG(TAG, "Get_Collecter_id  max %d min %d \r\n", *max, *min);
									LOG_DEBUG(TAG, "Get_Collecter_id  collector return a2 = %d\r\n",cJSON_GetObjectItem(JsonNode3, "a2")->valueint );
									return cJSON_GetObjectItem(JsonNode3, "a2")->valueint;
								}
						}
					}
				}
			}
		}
	}
	return 0;
}



#define GREATER_THAN_MAX_OPEN 10
#define GREATER_THAN_MAX_CLOSE 20
#define LESS_THAN_MIN_OPEN 10
#define LESS_THAN_MIN_CLOSE 20
void  auto_control_dev(void)
{

	int i = 0, j = 0, k = 0, temp, temp_max, temp_min, temp_local_data;
	cJSON *JsonRoot1, *JsonOption2, *JsonNode3, *JsonData4;
	char buf_cmd[32];
	
	memset(buf_cmd, 0 ,sizeof(buf_cmd));

	if((JsonRoot1 = GetJsonObject(JSON_CONFIG_FILE)) == NULL)
	{
		LOG_ERROR(TAG, "auto_control_dev error jsonroot is null\r\n");
	}else{
		JsonOption2 = cJSON_GetObjectItem(JsonRoot1, "controller");
		if(JsonOption2 == NULL)
		{
			LOG_ERROR(TAG, "auto_control_dev  controller  is null\r\n");
		}else{
			for(i = 0; i < cJSON_GetArraySize(JsonOption2); i++)
			{
				JsonNode3 = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i),"dev");
				if(JsonNode3 == NULL)
				{	
					LOG_ERROR(TAG, "auto_control_dev  dev  is null\r\n");
				}else{
					for(j = 0; j < cJSON_GetArraySize(JsonNode3); j++)
					{
						JsonData4 = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"acon");
						if(JsonData4 == NULL)
						{	
							LOG_ERROR(TAG, "auto_control_dev  acon  is null\r\n");
						}else{

							if(JsonData4->valueint != 1)
								continue;
							temp = Get_Collecter_id(JsonRoot1, cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"a6")->valueint,
									cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"glsen")->valueint, &temp_max,&temp_min);

							if(temp != 0)
							{
								temp_local_data =  Get_Collecter_glsen_data(temp, 	cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"glsen")->valueint);

								if(temp_local_data == 0) // Get_Collecter_glsen_data error
									continue;
								memset(buf_cmd, 0 ,sizeof(buf_cmd));
								buf_cmd[0] = CMD_HEAD0;
								buf_cmd[1] = CMD_HEAD1;

								buf_cmd[2] = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i),"conid")->valueint; // controller id

								buf_cmd[3] = 0x05; // cmd
								buf_cmd[4] = 0x02; // len 

								buf_cmd[5] = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"devid")->valueint;
								LOG_DEBUG(TAG, "auto_control_dev glsen_data = %d\r\n", temp_local_data);
								if(temp_max <= temp_min)
									continue;
								if(temp_local_data > temp_max)
								{

									if(	cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"re1") != NULL)
									{
										if(atoi(cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"re1")->valuestring) == GREATER_THAN_MAX_OPEN )
										{
											if((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq") != NULL) &&(cJSON_GetArraySize((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq"))) == 1 ))
											{
												LOG_DEBUG(TAG, "GREATER_THAN_MAX_OPEN get_huganqi_status start buf[2] = %d hgq = %d\r\n", buf_cmd[2], cJSON_GetArrayItem((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq")), 0)->valueint );
												if( get_huganqi_status(buf_cmd[2],cJSON_GetArrayItem((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq")), 0)->valueint) == 1)
													continue;
												buf_cmd[6] = 0x22;
											}
											else if((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq") != NULL) &&(cJSON_GetArraySize((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq"))) > 1 ))
											{
												LOG_DEBUG(TAG, "GREATER_THAN_MAX_OPEN get_huganqi_status start buf[2] = %d hgq = %d\r\n", buf_cmd[2], cJSON_GetArrayItem((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq")), 0)->valueint );
												if( get_huganqi_status(buf_cmd[2],cJSON_GetArrayItem((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq")), 0)->valueint) == 1)
													continue;

												buf_cmd[6] = 0xee;
											}
											LOG_DEBUG(TAG, "re1 GREATER_THAN_MAX_OPEN\r\n");
										}
										else if(atoi(cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"re1")->valuestring) == GREATER_THAN_MAX_CLOSE )
										{
											if((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq") != NULL) &&(cJSON_GetArraySize((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq"))) == 1 ))
											{
												LOG_DEBUG(TAG, "GREATER_THAN_MAX_CLOSE get_huganqi_status start buf[2] = %d hgq = %d\r\n", buf_cmd[2], cJSON_GetArrayItem((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq")), 0)->valueint );
												if( get_huganqi_status(buf_cmd[2],cJSON_GetArrayItem((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq")), 0)->valueint) == 0)
													continue;
												buf_cmd[6] = 0x33;
											}
											else if((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq") != NULL) &&(cJSON_GetArraySize((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq"))) > 1 ))
											{
												LOG_DEBUG(TAG, "GREATER_THAN_MAX_CLOSE get_huganqi_status start buf[2] = %d hgq = %d\r\n", buf_cmd[2], cJSON_GetArrayItem((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq")), 1)->valueint );
												if( get_huganqi_status(buf_cmd[2],cJSON_GetArrayItem((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq")), 1)->valueint) == 1)
													continue;

												buf_cmd[6] = 0x11;
											}
											LOG_DEBUG(TAG, "re1 GREATER_THAN_MAX_CLOSE\r\n");
										}
									}

								}else if(temp_local_data < temp_min)
								{
									if(	cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"re2") != NULL)
									{
										if(atoi(cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"re2")->valuestring) == LESS_THAN_MIN_OPEN )
										{
											if((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq") != NULL) &&(cJSON_GetArraySize((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq"))) ==1 ))
											{
												LOG_DEBUG(TAG, "LESS_THAN_MIN_OPEN get_huganqi_status start buf[2] = %d hgq = %d\r\n", buf_cmd[2], cJSON_GetArrayItem((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq")), 0)->valueint );
												if( get_huganqi_status(buf_cmd[2],cJSON_GetArrayItem((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq")), 0)->valueint) == 1)
													continue;

												buf_cmd[6] = 0x22;
											}
											else if((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq") != NULL) &&(cJSON_GetArraySize((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq"))) > 1 ))
											{
												LOG_DEBUG(TAG, "LESS_THAN_MIN_OPEN get_huganqi_status start buf[2] = %d hgq = %d\r\n", buf_cmd[2], cJSON_GetArrayItem((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq")), 0)->valueint );
												if( get_huganqi_status(buf_cmd[2],cJSON_GetArrayItem((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq")), 0)->valueint) == 1)
													continue;
												buf_cmd[6] = 0xee;
											}
											LOG_DEBUG(TAG, "re1 LESS_THAN_MIN_OPEN\r\n");
										}
										else if(atoi(cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"re2")->valuestring) == LESS_THAN_MIN_CLOSE )
										{
											if((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq") != NULL) &&(cJSON_GetArraySize((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq"))) ==1 ))
											{
												LOG_DEBUG(TAG, "LESS_THAN_MIN_CLOSE get_huganqi_status start buf[2] = %d hgq = %d\r\n", buf_cmd[2], cJSON_GetArrayItem((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq")), 0)->valueint );
												if( get_huganqi_status(buf_cmd[2],cJSON_GetArrayItem((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq")), 0)->valueint) == 0)
													continue;

												buf_cmd[6] = 0x33;
											}
											else if((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq") != NULL) &&(cJSON_GetArraySize((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq"))) > 1 ))
											{
												LOG_DEBUG(TAG, "LESS_THAN_MIN_CLOSE get_huganqi_status start buf[2] = %d hgq = %d\r\n", buf_cmd[2], cJSON_GetArrayItem((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq")), 1)->valueint );
												if( get_huganqi_status(buf_cmd[2],cJSON_GetArrayItem((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j),"hgq")), 1)->valueint) == 1)
													continue;
// error
												buf_cmd[6] = 0x11;
											}
											LOG_DEBUG(TAG, "re1 LESS_THAN_MIN_CLOSE\r\n");
										}
									}
								
								}

								temp = CRC16_Change(buf_cmd, 7);
								buf_cmd[5 + buf_cmd[4]] = (temp&0x0000ff00)>>8;
								buf_cmd[6 + buf_cmd[4]] = (temp&0x000000ff);
								buf_cmd[7 + buf_cmd[4]] = 0xbf;

								printf("auto_control_dev :");
								for(k = 0; k < strlen(buf_cmd); k ++)
								{
									printf("%02x ", buf_cmd[k]);
								}
								printf("\r\n");
								write(fd, buf_cmd, strlen(buf_cmd));
								sleep(2);
							}

						}
					}

				}
			}
		}
	}

}
int Send_Function(int fd,int id, unsigned char Buf_Send_Type, unsigned char buf[], int len)
{
	int  i = 0, j = 0,temp = 0;
	int nread = 0;
	unsigned char flag_write_fd_again_number = 0;


	char *buf_strcat_bad_string;
	unsigned char buf_strcat_bad_string_len = 0;

	//	fd_set readlist, readlistold;
	//	struct timeval timeout;
	char re;

	bzero(Buf_Send, sizeof(Buf_Send));
	Buf_Send[0] = 0xa5;
	Buf_Send[1] = 0x5a;
	Buf_Send[2] = id;
	switch(Buf_Send_Type)
	{
	case BUF_CONTROLLER_TYPE: // 2
		Buf_Send[3] = 0x05;	
		break;

	case BUF_COLLECTOR_TYPE: // 0
		Buf_Send[3] = 0x03;	
		break;

	case BUF_TRANSFORMER_TYPE: // 1
		Buf_Send[3] = 0x02;	
		break;
	}

	Buf_Send[4] = len;
	if(Buf_Send_Type != BUF_TRANSFORMER_TYPE)
	{
		strncat(Buf_Send, buf, len);
	}else
		Buf_Send[5] = 0;
	LOG_DEBUG(TAG, "Buf_Send SIZE = %d\r\n", (int)(Buf_Send[4]));

	temp = CRC16_Change(Buf_Send, 5 + Buf_Send[4]);
	Buf_Send[5 + Buf_Send[4]] = (temp&0x0000ff00)>>8;
	Buf_Send[6 + Buf_Send[4]] = (temp&0x000000ff);
	Buf_Send[7 + Buf_Send[4]] = 0xbf;

	LOG_DEBUG(TAG, "Buf_Send all SIZE = %d\r\n", (int)( 8 + len));



	buf_strcat_bad_string = (char *)malloc(256);
	//	FD_ZERO(&readlistold);
	//	FD_SET(fd, &readlistold);

	LOG_DEBUG(TAG, "Buf_Send zigbee:");
write_fd_again:
	for(i = 0; i <  8 + len  ; i++)
		printf("%02x ", Buf_Send[i]);
	printf("\r\n");
	memset(buf_strcat_bad_string, 0, 256);
	buf_strcat_bad_string_len = 0;
	write(fd,Buf_Send, 8 + len);



	//	timeout.tv_sec = 5;
	//	timeout.tv_usec = 0;


	while(1)
	{
#if 0
		readlist = readlistold;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		if((re = select(fd + 1, &readlist, NULL, NULL, &timeout)) < 0)
			LOG_ERROR(TAG, "timeout set error\r\n");
		LOG_INFO(TAG, "time is  %d s: %d us\r\n", timeout.tv_sec, timeout.tv_usec);	
		if(re == 0)
		{
			LOG_ERROR(TAG, "timeout and break\r\n");
			break;
		}

		if(FD_ISSET(fd, &readlist))
#endif
		{
			memset(Buf_Recv,0,sizeof(Buf_Recv));	
			nread = read(fd, Buf_Recv, sizeof(Buf_Recv));//读串口
			if (nread > 0){
				LOG_DEBUG(TAG,"DATALen=%d\n",nread); 

				LOG_DEBUG(TAG, "Buf_recv zigbee:");
				for(i = 0; i < nread; i++)
					printf("%02x ", Buf_Recv[i]);
				printf("\r\n");

				temp = CRC16_Change(Buf_Recv, 5 + Buf_Recv[4]);

				if(	(((temp&0x0000ff00)>>8) == Buf_Recv[5 + Buf_Recv[4]])	&&  ((temp&0x000000ff) == Buf_Recv[6 + Buf_Recv[4]]) )
				{
					LOG_DEBUG(TAG, "DATA is perfect, crc16 ok\r\n");
					Create_Sharemem_Json(Buf_Recv, nread);
					break;
				}else{
					strncat(buf_strcat_bad_string, Buf_Recv,nread);
					buf_strcat_bad_string_len += nread;
					//		LOG_ERROR(TAG, "buf_strcat_bad_string strlen = %d\r\n", strlen(buf_strcat_bad_string));
					for(i = 0; i < strlen(buf_strcat_bad_string); i++)
						printf("%02x ", buf_strcat_bad_string[i]);
					printf("\r\n");
					temp = CRC16_Change(buf_strcat_bad_string, 5 + buf_strcat_bad_string[4]);

					if(	(((temp&0x0000ff00)>>8) == buf_strcat_bad_string[5 + buf_strcat_bad_string[4]])	&& 
							((temp&0x000000ff) == buf_strcat_bad_string[6 + buf_strcat_bad_string[4]]) )
					{
						LOG_DEBUG(TAG, "DATA is perfect, crc16 ok because of buf_strcat_bad_string\r\n");
						Create_Sharemem_Json(buf_strcat_bad_string,buf_strcat_bad_string_len);
						break;
					}

					LOG_ERROR(TAG, "DATA is error because of crc16\r\n");
					if(strlen(buf_strcat_bad_string) >= 200)
					{
						memset(buf_strcat_bad_string, 0, 256);
						buf_strcat_bad_string_len = 0;
					}
				}
			}
			else if(nread == 0)
				LOG_ERROR(TAG, "DATA nread  = 0 end of fd\r\n"); 
			else
			{

				if(((j % 100) == 0) &&(j > 99))
					LOG_ERROR(TAG, "DATA nread  < 0  j = %d\r\n", j);
				if(j ++ > 500)
				{
					j  = 0;
					if(flag_write_fd_again_number++ < 2)
						goto write_fd_again;
					LOG_ERROR(TAG, "DATA nread  < 0  20 * 500us * 3times\r\n");
					break;
				}
			}
			usleep(20);
		}
	}

	free(buf_strcat_bad_string);
	return 1;
}

unsigned char send_just_for_transform(char *buf, int len) // todo 0630
{





}

unsigned char buf_control_dev_by_time[32][6] = {0}; // 32 * 6 be sure unsigned char todo 
char  control_dev_by_time(int fd)
{

	cJSON *JsonRoot1, *JsonOption2, *JsonNode3, *JsonData4;
	cJSON *json_trans1, *json_trans2, *json_trans3;
	char buf_all_data[32];
	int  i = 0, j = 0, k = 0, temp = 0, x = 0, m = 0; 
	struct tm  *time_nowtm;
	time_t timer;
	int nread;
	char Buf_Recv[32];
	unsigned char flag_three_times = 0;
	struct timeval time_begin;
	struct timeval time_end;
	int time_elapsed = 0,time_elapsed_last;
	int hours, minutes, seconds;
	long  st1_time, et1_time, st2_time, et2_time, st3_time, et3_time, now_time;
	LOG_DEBUG(TAG, "enter control_dev_by_time \r\n");
	if((JsonRoot1 = GetJsonObject(JSON_CONFIG_FILE)) == NULL)
	{
		LOG_ERROR(TAG, "control_dev_by_time error jsonroot is null");
	}else{
		JsonOption2 = cJSON_GetObjectItem(JsonRoot1, "controller");
		if(JsonOption2 == NULL)
		{
			LOG_ERROR(TAG, "GetJsonObject control_dev_by_time collector  is null");
		}else{
			if(cJSON_GetArraySize(JsonOption2) != 0)
			{
				for(i = 0; i < cJSON_GetArraySize(JsonOption2); i ++)
				{
					JsonNode3 = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2 ,i), "dev");
					if(JsonNode3 !=  NULL)
					{
						if(cJSON_GetArraySize(JsonNode3) != 0)
						{
							for(j = 0; j < cJSON_GetArraySize(JsonNode3); j ++)
							{
								JsonData4  = cJSON_GetArrayItem(JsonNode3, j);
								if(JsonData4 != NULL)
								{
									if((cJSON_GetObjectItem(JsonData4, "acon")!= NULL)&&(cJSON_GetObjectItem(JsonData4, "acon")->valueint == 2))
									{
										if((st1_time = cJSON_Get_ST_ET_time(JsonData4, "st1")) > 0)
											LOG_DEBUG(TAG, "st1_time = %ld\r\n", st1_time);
										if((st2_time = cJSON_Get_ST_ET_time(JsonData4, "st2")) > 0)
											LOG_DEBUG(TAG, "st2_time = %ld\r\n", st2_time);
										if((st3_time = cJSON_Get_ST_ET_time(JsonData4, "st3")) > 0)
											LOG_DEBUG(TAG, "st3_time = %ld\r\n", st3_time);

										if((et1_time = cJSON_Get_ST_ET_time(JsonData4, "et1")) > 0)
											LOG_DEBUG(TAG, "et1_time = %ld\r\n", et1_time);
										if((et2_time = cJSON_Get_ST_ET_time(JsonData4, "et2")) > 0)
											LOG_DEBUG(TAG, "et2_time = %ld\r\n", et2_time);
										if((et3_time = cJSON_Get_ST_ET_time(JsonData4, "et3")) > 0)
											LOG_DEBUG(TAG, "et3_time = %ld\r\n", et3_time);

										time(&(timer));
										time_nowtm= localtime(&timer);

										//									LOG_DEBUG(TAG, "hour = %d\r\n",time_nowtm->tm_hour );
										//									LOG_DEBUG(TAG, "minute = %d\r\n",time_nowtm->tm_min );
										//									LOG_DEBUG(TAG, "second = %d\r\n",time_nowtm->tm_sec );

										now_time = time_nowtm->tm_hour * 3600 + time_nowtm->tm_min * 60 + time_nowtm->tm_sec;
										LOG_DEBUG(TAG, "now_time = %d\r\n", now_time);

										if(((now_time > st1_time)&&(now_time < et1_time))||((now_time > st2_time)&&(now_time < et2_time))
												|| ((now_time > st3_time)&&(now_time < et3_time)))
										{
											LOG_DEBUG(TAG,"nowtime  should  open dev\r\n");
											if(cJSON_GetArraySize(cJSON_GetObjectItem(JsonData4, "hgq"))  == 1) // three relay
											{
												LOG_DEBUG(TAG, "get_huganqi_status a6 =%d hgq[0] = %d\r\n",cJSON_GetObjectItem(JsonData4, "a6")->valueint , cJSON_GetArrayItem(cJSON_GetObjectItem(JsonData4, "hgq"), 0)->valueint);	
												if(get_huganqi_status(cJSON_GetObjectItem(JsonData4, "a6")->valueint , cJSON_GetArrayItem(cJSON_GetObjectItem(JsonData4, "hgq"), 0)->valueint) == 1)
												{
													LOG_DEBUG(TAG, "control_dev_by_time data cmd is send and transformer ok");
													continue;
												}
												else
												{
													buf_all_data[0] = 0xa5;
													buf_all_data[1] = 0x5a;
													buf_all_data[2] = cJSON_GetObjectItem(JsonData4, "a6")->valueint;   
													buf_all_data[3] = 0x05;   

													buf_all_data[4] = 0x02;
													buf_all_data[5] = cJSON_GetObjectItem(JsonData4, "devid")->valueint;
													buf_all_data[6] = 0x22;
													temp = CRC16_Change(buf_all_data, 7 );
													buf_all_data[7] = (temp &0x0000ff00 )>>8;
													buf_all_data[8] = (temp &0x000000ff);
													buf_all_data[9] = 0xbf;

													LOG_DEBUG(TAG, "control_dev_by_time data -->\t");
													for(x = 0; x < 10; x ++)
														printf("%02x ", buf_all_data[x]);
													printf("\r\n");

													write(fd, buf_all_data,  10); //考虑数据中存在/0  长度会发生改变
													sleep(1);
												}

											}else if(cJSON_GetArraySize(cJSON_GetObjectItem(JsonData4, "hgq")) == 4) // three relay
											{

												LOG_DEBUG(TAG, "get_huganqi_status a6 =%d hgq[0] = %d\r\n",cJSON_GetObjectItem(JsonData4, "a6")->valueint , cJSON_GetArrayItem(cJSON_GetObjectItem(JsonData4, "hgq"), 0)->valueint);	
										
													buf_all_data[0] = 0xa5;
													buf_all_data[1] = 0x5a;
													buf_all_data[2] = cJSON_GetObjectItem(JsonData4, "a6")->valueint;   
													buf_all_data[3] = 0x05;   

													buf_all_data[4] = 0x02;
													buf_all_data[5] = cJSON_GetObjectItem(JsonData4, "devid")->valueint;
													buf_all_data[6] = 0xee;
													temp = CRC16_Change(buf_all_data, 7 );
													buf_all_data[7] = (temp &0x0000ff00 )>>8;
													buf_all_data[8] = (temp &0x000000ff);
													buf_all_data[9] = 0xbf;

													LOG_DEBUG(TAG, "control_dev_by_time data -->\t");
													for(x = 0; x < 10; x ++)
														printf("%02x ", buf_all_data[x]);
													printf("\r\n");

													write(fd, buf_all_data,  10); //考虑数据中存在/0  长度会发生改变
													sleep(1);
											


											}
										}

									 else
									 {
											LOG_DEBUG(TAG,"nowtime  should  close dev\r\n");
											if(cJSON_GetArraySize(cJSON_GetObjectItem(JsonData4, "hgq"))  == 1) // three relay
											{
												LOG_DEBUG(TAG, "get_huganqi_status a6 =%d hgq[0] = %d\r\n",cJSON_GetObjectItem(JsonData4, "a6")->valueint , cJSON_GetArrayItem(cJSON_GetObjectItem(JsonData4, "hgq"), 0)->valueint);	
												if(get_huganqi_status(cJSON_GetObjectItem(JsonData4, "a6")->valueint , cJSON_GetArrayItem(cJSON_GetObjectItem(JsonData4, "hgq"), 0)->valueint) == 0)
												{
													LOG_DEBUG(TAG, "control_dev_by_time data cmd is send and transformer ok");
													continue;
												}
												else
												{
													buf_all_data[0] = 0xa5;
													buf_all_data[1] = 0x5a;
													buf_all_data[2] = cJSON_GetObjectItem(JsonData4, "a6")->valueint;   
													buf_all_data[3] = 0x05;   

													buf_all_data[4] = 0x02;
													buf_all_data[5] = cJSON_GetObjectItem(JsonData4, "devid")->valueint;
													buf_all_data[6] = 0x33;
													temp = CRC16_Change(buf_all_data, 7 );
													buf_all_data[7] = (temp &0x0000ff00 )>>8;
													buf_all_data[8] = (temp &0x000000ff);
													buf_all_data[9] = 0xbf;

													LOG_DEBUG(TAG, "control_dev_by_time data -->\t");
													for(x = 0; x < 10; x ++)
														printf("%02x ", buf_all_data[x]);
													printf("\r\n");

													write(fd, buf_all_data,  10); //考虑数据中存在/0  长度会发生改变
													sleep(1);
												}

											}else if(cJSON_GetArraySize(cJSON_GetObjectItem(JsonData4, "hgq")) == 4) // three relay
											{

												LOG_DEBUG(TAG, "get_huganqi_status a6 =%d hgq[0] = %d\r\n",cJSON_GetObjectItem(JsonData4, "a6")->valueint , cJSON_GetArrayItem(cJSON_GetObjectItem(JsonData4, "hgq"), 0)->valueint);	

												buf_all_data[0] = 0xa5;
												buf_all_data[1] = 0x5a;
												buf_all_data[2] = cJSON_GetObjectItem(JsonData4, "a6")->valueint;   
												buf_all_data[3] = 0x05;   

												buf_all_data[4] = 0x02;
												buf_all_data[5] = cJSON_GetObjectItem(JsonData4, "devid")->valueint;
												buf_all_data[6] = 0x11;
												temp = CRC16_Change(buf_all_data, 7 );
												buf_all_data[7] = (temp &0x0000ff00 )>>8;
												buf_all_data[8] = (temp &0x000000ff);
												buf_all_data[9] = 0xbf;

												LOG_DEBUG(TAG, "control_dev_by_time data -->\t");
												for(x = 0; x < 10; x ++)
													printf("%02x ", buf_all_data[x]);
												printf("\r\n");

												write(fd, buf_all_data,  10); //考虑数据中存在/0  长度会发生改变
												sleep(1);

											}
										}



									}
								}
							}
						}
					}
				}
			}
		}
	}
}
int Task_TTYS1_Zigbee_Dev(void)
{
	int ret = 0;
	pthread_t tid;

	ret = pthread_create(&tid, NULL, thread_Task_TTYS1_Zigbee_Dev, NULL);
	if (ret != 0) {
		LOG_ERROR(TAG, "pthread_create error : %d\n", ret);
		ret = -1;
	}
	//	pthread_join(tid,NULL);
	LOG_DEBUG(TAG, "Task_TTYS1_Zigbee_Dev  start\n");

	return ret;
}
