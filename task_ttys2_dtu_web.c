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

#include "json_function.h"

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpointer.h>

#include <pthread.h>

#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include "task_ttys2_dtu_web.h"

static const char TAG[] = "Task_TTYS2_Dtu_Web";

static pthread_mutex_t pthread_mutex_ttys2_this = PTHREAD_MUTEX_INITIALIZER;
unsigned char Buf_Recv[512];
unsigned char flag_lost_connect_to_web = 2; // formal 
cJSON *jsonroot = NULL;

int fd_ttys2, nset;


extern unsigned char flag_lost_connect_to_web; 


void send_sqlite_data_to_web(void)
{

	pthread_mutex_lock(&pthread_mutex_ttys2_this);
	if(flag_lost_connect_to_web == 0)
		get_data_from_sqlite_to_web(fd_ttys2);
	pthread_mutex_unlock(&pthread_mutex_ttys2_this);

}

static void *thread_Task_TTYS2_Dtu_Web(void *data)
{
	int  i = 0, j = 0,temp = 0;
	int len = 0 ;
	unsigned char temp_buf[16] = {0x00};
	int nread = 0;
	char *buf_strcat_for_more_Data;
	struct timeval time_begin;
	struct timeval time_end;
	int time_elapsed = 0,time_elapsed_last;
	LOG_DEBUG(TAG, "%s start \r\n", Task_TTYS2_DTU );
	LOG_DEBUG(TAG, "%s start gettid = %u %u \r\n", Task_TTYS2_DTU,  pthread_self(), syscall(SYS_gettid));
	jsonroot = cJSON_CreateObject();
	if(jsonroot == NULL){
		LOG_ERROR(TAG, "jsonroot is NULL %s \r\n");
		return 0;
	}
#if  1
	if( GetJsonObject(JSON_CONFIG_FILE) ==  NULL)
	{
		if(Create_Pkgs(jsonroot))
			SaveJsonToFile(jsonroot,JSON_CONFIG_FILE );
	}else

		jsonroot = GetJsonObject(JSON_CONFIG_FILE);
#else
#endif
	//	fd = open(argv[1], O_RDWR);//打开串口
	LOG_DEBUG(TAG,"start try to open dev\r\n");
	fd_ttys2 = open(Task_TTYS2_DTU, O_RDWR| O_NONBLOCK);//打开串口
	if (fd_ttys2 == -1)
	{
		LOG_ERROR(TAG,"open %s FAIL\r\n", Task_TTYS2_DTU);
		pthread_exit(NULL);
	}

	LOG_DEBUG(TAG,"open ok\r\n");
	nset = set_opt(fd_ttys2,9600, 8, 'N', 1);//设置串口属性
	if (nset == -1)
	{
		LOG_ERROR(TAG, "set %s  FAIL\r\n", Task_TTYS2_DTU);
		//exit(1);	
		pthread_exit(NULL);
	}
	LOG_DEBUG(TAG, "set %s  ok\r\n",Task_TTYS2_DTU);

	buf_strcat_for_more_Data = (char *)malloc(512);
	memset(buf_strcat_for_more_Data, 0, 512);
	gettimeofday(&time_begin, NULL);

	while(1)
	{
		memset(Buf_Recv,0,sizeof(Buf_Recv));	
		nread = read(fd_ttys2, Buf_Recv, sizeof(Buf_Recv));//读串口
		if (nread > 0){
			LOG_DEBUG(TAG,"while 1 read from web DATALen=%d\n",nread);
			LOG_DEBUG(TAG,"while 1 Buf_Recv[5] + 9=%d\n", (Buf_Recv[4] << 8) +  Buf_Recv[5] + 9);
		
			gettimeofday(&time_begin, NULL);
			flag_lost_connect_to_web = 0;
			if((nread ) == (Buf_Recv[5] + 9))
			{
				if((Buf_Recv[3] != 2)&&(Buf_Recv[3] != 3))
				{
					LOG_DEBUG(TAG, "Buf_Recv DATA from web:");
					for(i = 0; i < nread; i++)
						printf("%02x ", Buf_Recv[i]);
					printf("\r\n");
				}
				Recv_Function(fd_ttys2, Buf_Recv, (Buf_Recv[4] << 8) + Buf_Recv[5] + 9);
				memset(buf_strcat_for_more_Data, 0, 512);
				len = 0;
			}
			else
			{
			//	strncat(buf_strcat_for_more_Data,Buf_Recv, nread);
			//	// when seek 0  return;

				for(i = 0; i < nread; i++)
					buf_strcat_for_more_Data[i + len] = Buf_Recv[i];


				len += nread;
				LOG_DEBUG(TAG,"len = %d\r\n", len);
				LOG_DEBUG(TAG, "Buf_Recv DATA from web:");
				for(i = 0; i < len; i ++)
					printf("%02x ", buf_strcat_for_more_Data[i]);
				printf("\r\n");
				if(buf_strcat_for_more_Data[len - 1] == 0xbf)
				{
					LOG_DEBUG(TAG, "use buf_strcat_for_more_Data enter good luck !!!\r\n");
					Recv_Function(fd_ttys2, buf_strcat_for_more_Data, len);
					memset(buf_strcat_for_more_Data, 0, 512);
					len = 0;
				}

				if(strlen(buf_strcat_for_more_Data) > 500){
					memset(buf_strcat_for_more_Data, 0, 512);
					len  = 0;
				}
			}
		}
		else if(nread == 0)
			LOG_ERROR(TAG, "DATA nread  = 0 end of fd_ttys2\r\n"); 
		else
		{
			gettimeofday(&time_end, NULL);	
			time_elapsed  = get_time_val_diff(&time_begin, &time_end); //ms

			if(time_elapsed > 30000) //120s ->  120000
				flag_lost_connect_to_web = 1;	

		}	;//	LOG_ERROR(TAG, "DATA nread  < 0\r\n"); 


		usleep(20);
	}
//	free(buf_strcat_for_more_Data);
	close(fd_ttys2);
	return 0;
}

int Recv_Function(int fd_ttys2, unsigned char buf[], int len)
{
	int  i = 0, j = 0, k = 0,temp = 0;
	int nread = 0;
	int crc_data_len  = 0;
	int buf_int[10];
	cJSON *jsontemp = NULL ,*NextJsonData = NULL;
	cJSON *json_share_mem = NULL, *json_share_mem_Data = NULL;

	char buf_temp_s16[16];
	char buf_temp_s64[64];
	char flag_exist_item = 0; 


	crc_data_len = 6 + (buf[4] << 8) + buf[5];
	printf("crc_data_len = %d\r\n", crc_data_len);

	for(i = 0; i < len; i++)
		printf("%02x ", buf[i]);
	printf("\r\n");


	temp = CRC16_Change(buf, crc_data_len);

	LOG_DEBUG(TAG, "CRC16 %02x %02x\r\n",((temp&0x0000ff00)>>8), (temp&0x000000ff) ); 
	if(	(((temp&0x0000ff00)>>8) == buf[crc_data_len])	&&  ((temp&0x000000ff) == buf[crc_data_len + 1]) )
	{
		LOG_DEBUG(TAG, "DATA is perfect, crc16 ok\r\n"); 
	}else{
		LOG_ERROR(TAG, "DATA is error because of crc16\r\n");
		return 0;
	}


	Send_Web_All_Data.buf_all_data.header1 = CMD_HEAD0;
	Send_Web_All_Data.buf_all_data.header2 = CMD_HEAD1;
	Send_Web_All_Data.buf_all_data.id = buf[2];   // todo
	Send_Web_All_Data.buf_all_data.cmd = buf[3];



	LOG_DEBUG(TAG,"CMD=%02x %d\n",buf[3], buf[3]); 
	memset(Send_Web_All_Data.buf_all_data.Buf_Send_WebData , 0, sizeof(Send_Web_All_Data.buf_all_data.Buf_Send_WebData ));
	flag_exist_item = 0;

	switch(buf[3])
	{
	case CMD_READ_CAIJIQI:

		LOG_DEBUG(TAG,"CMD_READ_CAIJIQI\n"); 

		if(Read_Caijiqi_From_Json(buf[6], (Send_Web_All_Data.buf_all_data.Buf_Send_WebData), &crc_data_len) == 1)
		{
			LOG_DEBUG(TAG,"Read_Caijiqi_From_Json ok \r\n");
			for(i = 0; i < crc_data_len; i ++)
				printf("%02x  ", Send_Web_All_Data.buf_all_data.Buf_Send_WebData[i]);
			printf("\r\n");
		}
		else
			crc_data_len = 0;
		break;
	case CMD_READ_HUGANQI:
		if(Read_Huganqi_From_Json(buf[6], (Send_Web_All_Data.buf_all_data.Buf_Send_WebData), &crc_data_len) == 1)
		{
			LOG_DEBUG(TAG,"Read_Huganqi_From_Json ok \r\n");
			
			for(i = 0; i < crc_data_len; i ++)
				printf("%02x  ", Send_Web_All_Data.buf_all_data.Buf_Send_WebData[i]);

			printf("\r\n");
		}else
			return -1;
		break;
	case CMD_CONTROL_RELAY:
		for(i = 0; i < len; i++)
			printf("%02x ", buf[i]);
		printf("\r\n");
		crc_data_len = (buf[4]<<8) + buf[5];
		LOG_DEBUG(TAG,"CMD_CONTROL_RELAY crc_data_len = %d\r\n", crc_data_len);
		for(i = 0; i < (crc_data_len); i++)
		{
			Send_Web_All_Data.buf_all_data.Buf_Send_WebData[i] = buf[6 + i];
		}
		if(Save_Control_To_Json(buf[6], &(buf[8]) , buf[7]) == 1 )
		{
			LOG_DEBUG(TAG,"Save_Control_To_Json ok \r\n");
		}
	
		break;
	case CMD_ADJUSTTIME:

		sprintf(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, "date 20%d.%d.%d-%d:%d:%d",buf[6],buf[7],buf[8],buf[9],buf[10],buf[11]);
		LOG_DEBUG(TAG, "CMD_ADJUSTTIME :%s\r\n",Send_Web_All_Data.buf_all_data.Buf_Send_WebData);
		system(Send_Web_All_Data.buf_all_data.Buf_Send_WebData);
		memset(Send_Web_All_Data.buf_all_data.Buf_Send_WebData , 0, sizeof(Send_Web_All_Data.buf_all_data.Buf_Send_WebData ));

		strncat(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, &(buf[6]), buf[5]);
		crc_data_len = 7;
		break;
	case CMD_RELOADPARAM:

		break;
	case CMD_DEL_CURRDATA:

		break;
	case CMD_DEL_PARAM:

		break;
	case CMD_READ_VER:

		sprintf(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, "%c%c%c", 
				cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"cver")->valueint,
				(cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"ver")->valueint) / 100,
				(cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"ver")->valueint) % 100
			   );
		//		printf("ver: %d, %d,%d\r\n",cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"cver")->valueint,\
		(cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"ver")->valueint) / 100,\
			(cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"ver")->valueint) % 100);

		crc_data_len = 3;

		break;
	case CMD_SET_VER:
		cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"cver")->valueint = buf[6];
		cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"cver")->valuedouble = buf[6];
		//
		cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"ver")->valueint = (buf[7] * 100) + buf[8];
		cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"ver")->valuedouble = (buf[7] * 100) + buf[8];

		sprintf(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, "%c%c%c", 
				cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"cver")->valueint,
				(cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"ver")->valueint) / 100,
				(cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"ver")->valueint) % 100
			   );
		SaveJsonToFile(jsonroot, JSON_CONFIG_FILE);	

		crc_data_len = 3;
		break;
	case CMD_SET_MENXIAN:

		jsontemp = cJSON_GetObjectItem(jsonroot,"collector");

		printf("jsontemp size = %d\r\n", cJSON_GetArraySize(jsontemp));

		for(i = 0; i < cJSON_GetArraySize(jsontemp); i++)
		{

			if(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"a2") != NULL)
			{
				if(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"a2")->valueint == buf[6])
				{

					cJSON_Change_IntNumber(cJSON_GetArrayItem(jsontemp, i),"temps",buf[7]);
					cJSON_Change_IntNumber(cJSON_GetArrayItem(jsontemp, i),"tempx",buf[8]);

					cJSON_Change_IntNumber(cJSON_GetArrayItem(jsontemp, i),"humiditys",buf[9]);
					cJSON_Change_IntNumber(cJSON_GetArrayItem(jsontemp, i),"humidityx",buf[10]);


					if(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"oilhumins") == NULL)
						cJSON_AddNumberToObject(cJSON_GetArrayItem(jsontemp, i),"oilhumins", (int)((buf[11] < 8) + buf[12]));
					else
						cJSON_Change_IntNumber(cJSON_GetArrayItem(jsontemp, i),"oilhumins", (int)(buf[11] << 8) + buf[12] );

					if(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"oilhuminx") == NULL)
						cJSON_AddNumberToObject(cJSON_GetArrayItem(jsontemp, i),"oilhuminx", (int)((buf[13] < 8) + buf[14]));
					else
						cJSON_Change_IntNumber(cJSON_GetArrayItem(jsontemp, i),"oilhuminx", (int)(buf[13] << 8) + buf[14] );


					cJSON_Change_IntNumber(cJSON_GetArrayItem(jsontemp, i),"co2s",(buf[15] << 8) + buf[16]);
					cJSON_Change_IntNumber(cJSON_GetArrayItem(jsontemp, i),"co2x",(buf[17] << 8) + buf[18]);


					cJSON_Change_IntNumber(cJSON_GetArrayItem(jsontemp, i),"oilHumiditys", buf[19]);
					cJSON_Change_IntNumber(cJSON_GetArrayItem(jsontemp, i),"oilHumidityx", buf[20]);

			


					sprintf(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
							cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"a2")->valueint, 			
							cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"temps")->valueint,
							cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"tempx")->valueint,
							cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"humiditys")->valueint,
							cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"humidityx")->valueint,
							(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"illumins")->valueint &0xff00 )>>8,
							(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"illumins")->valueint &0x00ff ),
							(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"illuminx")->valueint &0xff00 )>>8,
							(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"illuminx")->valueint &0x00ff ),
							(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"co2s")->valueint &0xff00 )>>8,
							(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"co2s")->valueint &0x00ff ),
							(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"co2x")->valueint &0xff00 )>>8,
							(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"co2x")->valueint &0x00ff ),
							(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"oilHumiditys")->valueint ),
							(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"oilHumidityx")->valueint )
						   ); 
	
					LOG_DEBUG(TAG, "CMD_SET_MENXIAN Send_Web_All_Data get ok: %s\r\n", Send_Web_All_Data.buf_all_data.Buf_Send_WebData);
					
					break;
				}
			}
		}

		LOG_DEBUG(TAG, "set menxian jsonroot= \r\n%s\r\n", cJSON_Print(jsonroot));
		SaveJsonToFile(jsonroot, JSON_CONFIG_FILE);	

		crc_data_len = 15;

		break;
	case CMD_GET_MENXIAN:

		jsontemp = cJSON_GetObjectItem(jsonroot,"collector");
		for(i = 0; i < cJSON_GetArraySize(jsontemp); i++)
		{

			if(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"a2")->valueint == buf[6])
			{	

				sprintf(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
						cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"a2")->valueint, 			
						cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"temps")->valueint,
						cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"tempx")->valueint,
						cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"humiditys")->valueint,
						cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"humidityx")->valueint,
						(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"illumins")->valueint &0xff00 )>>8,
						(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"illumins")->valueint &0x00ff ),
						(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"illuminx")->valueint &0xff00 )>>8,
						(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"illuminx")->valueint &0x00ff ),
						(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"co2s")->valueint &0xff00 )>>8,
						(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"co2s")->valueint &0x00ff ),
						(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"co2x")->valueint &0xff00 )>>8,
						(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"co2x")->valueint &0x00ff ),
						(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"oilHumiditys")->valueint ),
						(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"oilHumidityx")->valueint )
					   ); 

				break;
			}
		}

		crc_data_len = 15;

		break;
	case CMD_SET_DPINFO:



		json_share_mem_Data = cJSON_CreateObject();

		NextJsonData = cJSON_CreateObject();

		cJSON_AddNumberToObject(NextJsonData, "a2", buf[6]);
		cJSON_AddNumberToObject(json_share_mem_Data, "id", buf[6]); //share
		cJSON_AddNumberToObject(NextJsonData, "a4", buf[7]);
		cJSON_AddNumberToObject(NextJsonData, "a5", buf[8]);

		memset(buf_int, 0, sizeof(buf_int));
		buf_int[0] = buf[9];


		cJSON_AddItemToObject(NextJsonData, "a6", cJSON_CreateIntArray(buf_int, 1));

		memset(buf_int, 0, sizeof(buf_int));
		for(i = 0; i < buf[10]; i++)
		{
			buf_int[i] = buf[11 +i];
		}
		cJSON_AddItemToObject(NextJsonData, "a7", cJSON_CreateIntArray(buf_int, buf[10]));

		cJSON_AddItemToObject(json_share_mem_Data, "type",cJSON_CreateIntArray(buf_int, buf[10])); //share

		printf("NextJsonData = \r\n %s\r\n", cJSON_Print(NextJsonData));
		printf("json_share_mem_Data = \r\n %s\r\n", cJSON_Print(json_share_mem_Data));

		jsontemp = cJSON_GetObjectItem(jsonroot,"collector");
		for(i = 0; i < cJSON_GetArraySize(jsontemp); i++)
		{

			if(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "a2" )->valueint == cJSON_GetObjectItem(NextJsonData,"a2")->valueint)
			{	
				cJSON_ReplaceItemInArray(jsontemp, i, NextJsonData);
				flag_exist_item = 1;
				break;
			}
		}
		if(flag_exist_item != 1)
			cJSON_AddItemToArray(jsontemp, NextJsonData);

		flag_exist_item = 0;
		if((json_share_mem = (GetJsonObject(SHARE_MEM_FIEL))) == NULL)
		{
			LOG_ERROR(TAG, "GetJsonObject SHARE_MEM_FIEL error jsonroot is null");
		}else{

			if((cJSON_GetObjectItem(json_share_mem, "collector")) == NULL)
			{
				LOG_ERROR(TAG, "GetJsonObject SHARE_MEM_FIEL error collector is null");
			}else
			{

				for(i = 0; i < cJSON_GetArraySize(cJSON_GetObjectItem(json_share_mem, "collector")); i++ )
				{

					if(cJSON_GetObjectItem(cJSON_GetArrayItem(cJSON_GetObjectItem(json_share_mem, "collector"), i), "id")->valueint
							== cJSON_GetObjectItem(json_share_mem_Data, "id")->valueint)
					{

						cJSON_ReplaceItemInArray(cJSON_GetObjectItem(json_share_mem, "collector"), i, json_share_mem_Data);
						flag_exist_item = 1;
						break;
					}
				}
				if(flag_exist_item != 1)
					cJSON_AddItemToArray(cJSON_GetObjectItem(json_share_mem, "collector"),json_share_mem_Data);

				LOG_DEBUG(TAG, "json_share_mem = \r\n%s\r\n", cJSON_Print(json_share_mem));
				SaveJsonToFile(json_share_mem, SHARE_MEM_FIEL);
			}
		}



		SaveJsonToFile(jsonroot, JSON_CONFIG_FILE);
		crc_data_len = 0;
		break;      
	case CMD_GET_DPINFO:	
		jsontemp = cJSON_GetObjectItem(jsonroot,"collector");

		for(i = 0; i < cJSON_GetArraySize(jsontemp); i++)
		{

			if(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "a2" )->valueint == buf[6])
			{
				sprintf(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, "%c%c%c%c%c", 
						cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "a2" )->valueint, 	
						cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "a4" )->valueint, 
						cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "a5" )->valueint,  // a5 maybe 0
						cJSON_GetArrayItem(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "a6"), 0)->valueint, 
						cJSON_GetArraySize(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "a7"))); 

				for(j = 0; j < cJSON_GetArraySize(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "a7")); j ++)
				{
					//	   memset(buf_temp_s16, 0, sizeof(buf_temp_s16));
					//	   sprintf(buf_temp_s16,"%c", cJSON_GetArrayItem( cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "a7"), j)->valueint);
					//	   strncat(send_web_all_data.buf_all_data.buf_send_webdata, buf_temp_s16, 1);	
					Send_Web_All_Data.buf_all_data.Buf_Send_WebData[5 + j] = cJSON_GetArrayItem( cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "a7"), j)->valueint;
				}

				break;
			}
		}

		//		crc_data_len = strlen(Send_Web_All_Data.buf_all_data.Buf_Send_WebData);
		//		should make sure { it is  ok }

		printf("cJSON_GetArraySize %d\r\n", cJSON_GetArraySize(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "a7")));
		crc_data_len = 5 + cJSON_GetArraySize(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "a7")) ;
		printf("crc_data_len = %d\r\n", crc_data_len);
		for(i = 0; i < crc_data_len; i++)
		{
			printf("%02x\t", Send_Web_All_Data.buf_all_data.Buf_Send_WebData[i]);
		}
		printf("\r\n CMD_GET_DPINFO\r\n");
		break;        
	case CMD_DEL_DPINFO:

		jsontemp = cJSON_GetObjectItem(jsonroot,"collector");
		for(i = 0; i < cJSON_GetArraySize(jsontemp); i++)
		{

			if(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "a2" )->valueint == buf[6])
				cJSON_DeleteItemFromArray(jsontemp, i);
		}
		SaveJsonToFile(jsonroot, JSON_CONFIG_FILE);
		sprintf(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, "%c",buf[6]);
		crc_data_len = 1;



		if((json_share_mem = (GetJsonObject(SHARE_MEM_FIEL))) == NULL)
		{
			LOG_ERROR(TAG, "GetJsonObject SHARE_MEM_FIEL error jsonroot is null");
		}else{

			if((cJSON_GetObjectItem(json_share_mem, "collector")) == NULL)
			{
				LOG_ERROR(TAG, "GetJsonObject SHARE_MEM_FIEL error collector is null");
			}else
			{
				for(i = 0; i < cJSON_GetArraySize(cJSON_GetObjectItem(json_share_mem, "collector")); i++ )
				{

					if(cJSON_GetObjectItem(cJSON_GetArrayItem(cJSON_GetObjectItem(json_share_mem, "collector"), i), "id")->valueint
							== buf[6])
					{
						cJSON_DeleteItemFromArray(cJSON_GetObjectItem(json_share_mem, "collector"), i);
						LOG_DEBUG(TAG, "cjson del SHARE_MEM_FIEL collector id %d\r\n", buf[6]);
						break;
					}
				}
				LOG_DEBUG(TAG, "json_share_mem = \r\n%s\r\n", cJSON_Print(json_share_mem));
				SaveJsonToFile(json_share_mem, SHARE_MEM_FIEL);
			}
		}

		break;        
	case CMD_SET_DPCONTROL:

		NextJsonData = cJSON_CreateObject();

		json_share_mem_Data =cJSON_CreateObject();
		cJSON_AddNumberToObject(NextJsonData,"devid", 1); // todo
		cJSON_AddNumberToObject(json_share_mem_Data, "id", buf[6]); //share
		cJSON_AddNumberToObject(NextJsonData,"a6", buf[6]);
		cJSON_AddNumberToObject(NextJsonData,"type", buf[7]);
		cJSON_AddNumberToObject(NextJsonData,"acon", buf[8]);

		printf("\r\nlen1 = %d\r\n", buf[9]);
		memset(buf_int,  0, sizeof(buf_int));
		for(i = 0; i < buf[9]; i ++)
		{	
			buf_int[i] = buf[10 + i];

			printf("%d\t", buf_int[i]);
		}
		cJSON_AddItemToObject(NextJsonData,"relay", cJSON_CreateIntArray(buf_int,buf[9]));

		printf("\r\nlen2 = %d\r\n", buf[10 + buf[9]]);

		memset(buf_int,  0, sizeof(buf_int));
		for(i = 0; i < buf[10 + buf[9]]; i ++)
		{	
			buf_int[i] = buf[11 + buf[9] + i];
			printf("%d\t", buf_int[i]);
		}
		cJSON_AddItemToObject(NextJsonData,"hgq", cJSON_CreateIntArray(buf_int,buf[10 + buf[9]]));
		// todo time set

		printf("\r\nlen3 = %d\r\n", buf[11 + buf[9] + buf[10 + buf[9]]]);

		printf("%d\r\n",11 + buf[9] + buf[10 + buf[9]] );
		memset(buf_int,  0, sizeof(buf_int));

		cJSON_AddNumberToObject(NextJsonData, "glsen", buf[12 + buf[9] + buf[10 + buf[9]]]);

		memset(buf_temp_s64,  0, sizeof(buf_temp_s64));
		sprintf(buf_temp_s64, "%d:%d", (buf[1 + 12 + buf[9] + buf[10 + buf[9]]] &0xf0)>> 4,(buf[1 + 12 + buf[9] + buf[10 + buf[9]]] & 0x0f) );
		cJSON_AddStringToObject(NextJsonData, "st1", buf_temp_s64);

		memset(buf_temp_s64,  0, sizeof(buf_temp_s64));
		sprintf(buf_temp_s64, "%d:%d", (buf[2 + 12 + buf[9] + buf[10 + buf[9]]] &0xf0)>> 4,(buf[1 + 12 + buf[9] + buf[10 + buf[9]]] & 0x0f) );
		cJSON_AddStringToObject(NextJsonData, "et1", buf_temp_s64);

		memset(buf_temp_s64,  0, sizeof(buf_temp_s64));
		sprintf(buf_temp_s64, "%d:%d", (buf[3 + 12 + buf[9] + buf[10 + buf[9]]] &0xf0)>> 4,(buf[1 + 12 + buf[9] + buf[10 + buf[9]]] & 0x0f) );
		cJSON_AddStringToObject(NextJsonData, "st2", buf_temp_s64);

		memset(buf_temp_s64,  0, sizeof(buf_temp_s64));
		sprintf(buf_temp_s64, "%d:%d", (buf[4 + 12 + buf[9] + buf[10 + buf[9]]] &0xf0)>> 4,(buf[1 + 12 + buf[9] + buf[10 + buf[9]]] & 0x0f) );
		cJSON_AddStringToObject(NextJsonData, "et2", buf_temp_s64);

		memset(buf_temp_s64,  0, sizeof(buf_temp_s64));
		sprintf(buf_temp_s64, "%d:%d", (buf[5 + 12 + buf[9] + buf[10 + buf[9]]] &0xf0)>> 4,(buf[1 + 12 + buf[9] + buf[10 + buf[9]]] & 0x0f) );
		cJSON_AddStringToObject(NextJsonData, "st3", buf_temp_s64);

		memset(buf_temp_s64,  0, sizeof(buf_temp_s64));
		sprintf(buf_temp_s64, "%d:%d", (buf[6 + 12 + buf[9] + buf[10 + buf[9]]] &0xf0)>> 4,(buf[1 + 12 + buf[9] + buf[10 + buf[9]]] & 0x0f) );
		cJSON_AddStringToObject(NextJsonData, "et3", buf_temp_s64);


		printf("NextJsonData = \r\n%s\r\n", cJSON_Print(NextJsonData));


		jsontemp = cJSON_GetObjectItem(jsonroot,"controller");
		printf("jsontemp size = %d\r\n", cJSON_GetArraySize(jsontemp));

		for(i = 0; i < cJSON_GetArraySize(jsontemp); i++)
		{

			if(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "conid")->valueint == cJSON_GetObjectItem(NextJsonData, "a6")->valueint)
			{

				for(j = 0; j < cJSON_GetArraySize(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "dev")); j ++)
				{

					if(cJSON_GetObjectItem(cJSON_GetArrayItem(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "dev"), j),"devid")->valueint
							== cJSON_GetObjectItem(NextJsonData, "devid")->valueint )
					{

						cJSON_ReplaceItemInArray(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "dev"),j, NextJsonData);
						flag_exist_item = 1;
						break;
					}	

				}

				if( flag_exist_item != 1)
					cJSON_AddItemToArray(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "dev"), NextJsonData);

			}
		}

		if((json_share_mem = (GetJsonObject(SHARE_MEM_FIEL))) == NULL)
		{
			LOG_ERROR(TAG, "GetJsonObject SHARE_MEM_FIEL error jsonroot is null");
		}else{

			if((cJSON_GetObjectItem(json_share_mem, "controller")) == NULL)
			{
				LOG_ERROR(TAG, "GetJsonObject SHARE_MEM_FIEL error collector is null");
			}else
			{
				for(i = 0; i < cJSON_GetArraySize(cJSON_GetObjectItem(json_share_mem, "controller")); i++ )
				{
					if(cJSON_GetObjectItem(cJSON_GetArrayItem(cJSON_GetObjectItem(json_share_mem, "controller"), i), "id")->valueint
							== cJSON_GetObjectItem(json_share_mem_Data, "id")->valueint)
					{
						LOG_DEBUG(TAG, "cjson set SHARE_MEM_FIEL collector id %d\r\n", cJSON_GetObjectItem(json_share_mem_Data, "id")->valueint);
						flag_exist_item = 1;
						break;
					}
				}
				if(flag_exist_item != 1)
					cJSON_AddItemToArray(cJSON_GetObjectItem(json_share_mem, "controller"), json_share_mem_Data);
				LOG_DEBUG(TAG, "json_share_mem = \r\n%s\r\n", cJSON_Print(json_share_mem));
				SaveJsonToFile(json_share_mem, SHARE_MEM_FIEL);
			}
		}


		SaveJsonToFile(jsonroot, JSON_CONFIG_FILE);
		break;     
	case CMD_GET_DPCONTROL: // no use
		return -1;	
		break;     
	case CMD_GET_DPCONTROLV3:


		jsontemp = cJSON_GetObjectItem(jsonroot,"controller");
		printf("jsontemp size = %d\r\n", cJSON_GetArraySize(jsontemp));

		for(i = 0; i < cJSON_GetArraySize(jsontemp); i++)
		{

			printf("i = %d data = %d\r\n", i, cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"conid")->valueint );
			if(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"conid")->valueint == buf[6])
			{

				for(j = 0; j < cJSON_GetArraySize(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "dev")); j++)
				{
					printf("j = %d\r\n", j);

					printf("data  =%d buf[7] = %d\r\n", 
							cJSON_GetObjectItem(cJSON_GetArrayItem(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "dev"), j),"devid")->valueint,
							buf[7]	

						  );
					if(cJSON_GetObjectItem(cJSON_GetArrayItem(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "dev"), j),"devid")->valueint
							== buf[7])
					{
						NextJsonData = cJSON_GetArrayItem(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "dev"), j);

						sprintf(Send_Web_All_Data.buf_all_data.Buf_Send_WebData,
								"<?xml version=\"1.0\" encoding=\"gb2312\"?><message System=\"\" Ver=\"1.0\"><dev><d id=\"%d\" a6=\"%d\" type=\"%d\" acon=\"%d\" glsen =\"%d\" re1=\"%s\" re2=\"%s\" st1=\"%s\" et1=\"%s\" st2=\"%s\" et2=\"%s\" st3=\"%s\" et3=\"%s\" " ,

								cJSON_GetObjectItem(NextJsonData,"devid")->valueint,
								cJSON_GetObjectItem(NextJsonData,"a6")->valueint,
								cJSON_GetObjectItem(NextJsonData,"type")->valueint,
								cJSON_GetObjectItem(NextJsonData,"acon")->valueint,
								cJSON_GetObjectItem(NextJsonData,"glsen")->valueint,
								cJSON_GetObjectItem(NextJsonData,"re1")->valuestring,
								cJSON_GetObjectItem(NextJsonData,"re2")->valuestring,
								cJSON_GetObjectItem(NextJsonData,"st1")->valuestring,
								cJSON_GetObjectItem(NextJsonData,"et1")->valuestring,
								cJSON_GetObjectItem(NextJsonData,"st2")->valuestring,
								cJSON_GetObjectItem(NextJsonData,"et2")->valuestring,
								cJSON_GetObjectItem(NextJsonData,"st3")->valuestring,
								cJSON_GetObjectItem(NextJsonData,"et3")->valuestring
							   );


						bzero(buf_temp_s64, sizeof(buf_temp_s64));

						for(k = 0; k < cJSON_GetArraySize(cJSON_GetObjectItem(NextJsonData,"relay")); k++)
						{
							bzero(buf_temp_s16, sizeof(buf_temp_s16));
							sprintf(buf_temp_s16,"%d", cJSON_GetArrayItem(cJSON_GetObjectItem(NextJsonData,"relay"), k)->valueint);
							strcat(buf_temp_s64, buf_temp_s16);	

							if(k != ( cJSON_GetArraySize(cJSON_GetObjectItem(NextJsonData,"relay")) - 1) )
								strcat(buf_temp_s64, ",");	
						}
						strcat(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, "relay=\"");
						strcat(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, buf_temp_s64);
						strcat(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, "\" ");

						bzero(buf_temp_s64, sizeof(buf_temp_s64));

						for(k = 0; k < cJSON_GetArraySize(cJSON_GetObjectItem(NextJsonData,"hgq")); k++)
						{
							bzero(buf_temp_s16, sizeof(buf_temp_s16));
							sprintf(buf_temp_s16,"%d", cJSON_GetArrayItem(cJSON_GetObjectItem(NextJsonData,"hgq"), k)->valueint);
							strcat(buf_temp_s64, buf_temp_s16);	

							if(k != ( cJSON_GetArraySize(cJSON_GetObjectItem(NextJsonData,"hgq")) - 1) )
								strcat(buf_temp_s64, ",");	
						}

						strcat(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, "hgq=\"");
						strcat(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, buf_temp_s64);
						strcat(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, "\" ");

						//relay=\"1,238,2,17\" hgq=\"1,238\" 

						strcat(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, "/></dev></message>");
					}

				}

			}
		}


		printf("dpcontrol v3 cmd : %s\r\n", Send_Web_All_Data.buf_all_data.Buf_Send_WebData);
		crc_data_len =  strlen(Send_Web_All_Data.buf_all_data.Buf_Send_WebData) ;

		break;   
	case CMD_DEL_DPCONTROL: 
		jsontemp = cJSON_GetObjectItem(jsonroot,"controller");

		printf("jsontemp size = %d\r\n", cJSON_GetArraySize(jsontemp));
		for(i = 0; i < cJSON_GetArraySize(jsontemp); i++)
		{
			if(buf[6] == 0)
				cJSON_DeleteItemFromArray(jsontemp, i);
			else{

				if(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "conid")->valueint == buf[6])
				{
					cJSON_DeleteItemFromArray(jsontemp, i);
					break;
				}

			}
		}
		SaveJsonToFile(jsonroot, JSON_CONFIG_FILE);
		crc_data_len = 0;

		
		if((json_share_mem = (GetJsonObject(SHARE_MEM_FIEL))) == NULL)
		{
			LOG_ERROR(TAG, "GetJsonObject SHARE_MEM_FIEL error jsonroot is null");
		}else{

			if((cJSON_GetObjectItem(json_share_mem, "controller")) == NULL)
			{
				LOG_ERROR(TAG, "GetJsonObject SHARE_MEM_FIEL error collector is null");
			}else
			{
				for(i = 0; i < cJSON_GetArraySize(cJSON_GetObjectItem(json_share_mem, "controller")); i++ )
				{
					if(buf[6] == 0)
						cJSON_DeleteItemFromArray( cJSON_GetObjectItem(json_share_mem, "controller"), i);
					else
					{
						if(cJSON_GetObjectItem(cJSON_GetArrayItem(cJSON_GetObjectItem(json_share_mem, "controller"), i), "id")->valueint
								== buf[6])
						{
							cJSON_DeleteItemFromArray( cJSON_GetObjectItem(json_share_mem, "controller"), i);
							break;
						}
					}
				}
				LOG_DEBUG(TAG, "json_share_mem = \r\n%s\r\n", cJSON_Print(json_share_mem));
				SaveJsonToFile(json_share_mem, SHARE_MEM_FIEL);
			}
		}

		break;     
	case CMD_SET_DEVINFO:


		break;       
	case CMD_SET_DPCONTROLV3:

		LOG_DEBUG(TAG, "CMD_SET_DPCONTROLV3 \r\n");
		temp =  function_Cmd_Set_Dpcontrol_V3(buf,len, &i, &j);


		memset(Send_Web_All_Data.buf_all_data.Buf_Send_WebData , 0, sizeof(Send_Web_All_Data.buf_all_data.Buf_Send_WebData ));
		Send_Web_All_Data.buf_all_data.Buf_Send_WebData[0] = i; 
		Send_Web_All_Data.buf_all_data.Buf_Send_WebData[1] = j; 
		Send_Web_All_Data.buf_all_data.Buf_Send_WebData[2] = temp & 0x00ff; 
		Send_Web_All_Data.buf_all_data.Buf_Send_WebData[3] = (temp & 0xff00) >> 8 ; 

		crc_data_len = 4;
		break;   
	case CMD_DEL_DPCONTROLV3:

		LOG_DEBUG(TAG, "CMD_DEL_DPCONTROLV3 \r\n");
		temp = function_Cmd_Del_Dpcontrol_V3(buf, len);

		if(temp == 0 )
			crc_data_len = 0;
		else
		{
			crc_data_len = 1;
			Send_Web_All_Data.buf_all_data.Buf_Send_WebData[0] = temp; 
		}
		break;   
	case CMD_RELAODPARAM: //暂时未能使用

		break;       
	case CMD_QUIT:
		crc_data_len = 0;

		break;              
	case CMD_SET_IPANDPORT: //同 CMD_GET_IPANDPORT 但是，需要进行设计保存


		memset(buf_int, 0 , sizeof(buf_int));

		cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"port")->valueint = (buf[13] <<8) + buf[14];
		cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"port")->valuedouble = (buf[13] <<8) + buf[14];

		if(buf[6] == 4) // ipv4
		{

			for(i = 0; i < 4 ; i ++)
				buf_int[i] =  buf[i + 7];
			jsontemp =  cJSON_CreateIntArray(buf_int,4);

			sprintf(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, "%c%c%c%c%c%c%c%c",
					cJSON_GetArrayItem(jsontemp,0)->valueint,
					cJSON_GetArrayItem(jsontemp,1)->valueint,
					cJSON_GetArrayItem(jsontemp,2)->valueint,
					cJSON_GetArrayItem(jsontemp,3)->valueint,
					0,0,
					(cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"port")->valueint &0xff00) >>8,
					(cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"port")->valueint &0x00ff)
				   );
		}else if(buf[6] == 6) // ipv6
		{
			for(i = 0; i < 6 ; i ++)
				buf_int[i] =  buf[i + 7];
			jsontemp =  cJSON_CreateIntArray(buf_int,6);

			sprintf(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, "%c%c%c%c%c%c%c%c",
					cJSON_GetArrayItem(jsontemp,0)->valueint,
					cJSON_GetArrayItem(jsontemp,1)->valueint,
					cJSON_GetArrayItem(jsontemp,2)->valueint,
					cJSON_GetArrayItem(jsontemp,3)->valueint,
					cJSON_GetArrayItem(jsontemp,4)->valueint,
					cJSON_GetArrayItem(jsontemp,5)->valueint,
					(cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"port")->valueint &0xff00) >>8,
					(cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"port")->valueint &0x00ff)

				   );

		}

		cJSON_ReplaceItemInObject(cJSON_GetObjectItem(jsonroot,"config"),"ip", jsontemp);
#if 0
		printf("senddata:\r\n");
		for(i = 0; i < 8;i++)
			printf("%02x\t", Send_Web_All_Data.buf_all_data.Buf_Send_WebData[i]);
		printf("\r\n");
		printf("port =%d\r\n",	cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"port")->valueint );
		printf("define jsonroot =%s\r\n",cJSON_Print(jsonroot));
#endif

		SaveJsonToFile(jsonroot, JSON_CONFIG_FILE);	
		crc_data_len = 9;

		break;     
	case CMD_GET_IPANDPORT:
		jsontemp = cJSON_GetObjectItem((cJSON_GetObjectItem(jsonroot,"config")),"ip");

		if(cJSON_GetArraySize(jsontemp) == 4)
		{
			sprintf(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, "%c%c%c%c%c%c%c%c",
					cJSON_GetArrayItem(jsontemp,0)->valueint,
					cJSON_GetArrayItem(jsontemp,1)->valueint,
					cJSON_GetArrayItem(jsontemp,2)->valueint,
					cJSON_GetArrayItem(jsontemp,3)->valueint,
					0,0,
					(cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"port")->valueint &0xff00) >>8,
					(cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"port")->valueint &0x00ff)
				   );
		}
		else 	if(cJSON_GetArraySize(jsontemp) == 6)
		{
			sprintf(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, "%c%c%c%c%c%c%c%c",
					cJSON_GetArrayItem(jsontemp,0)->valueint,
					cJSON_GetArrayItem(jsontemp,1)->valueint,
					cJSON_GetArrayItem(jsontemp,2)->valueint,
					cJSON_GetArrayItem(jsontemp,3)->valueint,
					cJSON_GetArrayItem(jsontemp,4)->valueint,
					cJSON_GetArrayItem(jsontemp,5)->valueint,
					(cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"port")->valueint &0xff00) >>8,
					(cJSON_GetObjectItem(cJSON_GetObjectItem(jsonroot,"config"),"port")->valueint &0x00ff)

				   );
		}

		crc_data_len = 9;
#if 0
		for(i = 0; i < crc_data_len - 1; i ++)
			printf("%02x\t", Send_Web_All_Data.buf_all_data.Buf_Send_WebData[i]);
		printf("\r\n");
#endif
		break;     
	case CMD_CLEAR_ALLDEV:

		jsontemp = cJSON_GetObjectItem(jsonroot,"controller");
		if(jsontemp != NULL)
		{
			for(i = 0; i < cJSON_GetArraySize(jsontemp); i++)
			{
				cJSON_DeleteItemFromArray(jsontemp, i);
			}
			SaveJsonToFile(jsonroot, JSON_CONFIG_FILE);	

		}
		sprintf(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, "%c%c",0, 0);
		crc_data_len = 2;

		break;      
	case CMD_GET_MENXIANV3:

		if((buf[4] <<8 + buf[5]) >1)
		{
			// todo  
		}else{

			jsontemp = cJSON_GetObjectItem(jsonroot,"collector");

			printf("jsontemp size = %d\r\n", cJSON_GetArraySize(jsontemp));

			for(i = 0; i < cJSON_GetArraySize(jsontemp); i++)
			{

				if(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"a2")->valueint == buf[6])
				{
					sprintf(Send_Web_All_Data.buf_all_data.Buf_Send_WebData,
							"<?xml version=\"1.0\" encoding=\"gb2312\"?><message System=\"\\\" Ver=\\\"1.0\"><dp temps=\"%d\" tempx=\"%d\" humins=\"%d\" huminx=\"%d\" illumins=\"%d\" illuminx=\"%d\" co2s=\"%d\" co2x=\"%d\" oilhumins=\"%d\" oilhuminx=\"%d\"></dp><dev></dev></message>",

							cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"temps")->valueint, 
							cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"tempx")->valueint, 
							cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"humiditys")->valueint, 
							cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"humidityx")->valueint, 

							(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"illumins")->valueint),
							(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"illuminx")->valueint),

							cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"co2s")->valueint, 
							cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"co2x")->valueint, 

							cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"oilHumiditys")->valueint, 
							cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"oilHumidityx")->valueint);
				}
			}
			crc_data_len =  strlen(Send_Web_All_Data.buf_all_data.Buf_Send_WebData) ;
		}

		break;     
	default:
		LOG_DEBUG(TAG, "Recv_Function default !\r\n"); 
		break;

	}

	Send_Web_All_Data.buf_all_data.len_h = crc_data_len / 256;
	Send_Web_All_Data.buf_all_data.len_l = crc_data_len % 256;

	temp = CRC16_Change(Send_Web_All_Data.Buf_Send, crc_data_len  + 6 );
	Send_Web_All_Data.Buf_Send[crc_data_len + 6 ] = (temp &0x0000ff00 )>>8;
	Send_Web_All_Data.Buf_Send[crc_data_len + 7 ] = (temp &0x000000ff);
	Send_Web_All_Data.Buf_Send[crc_data_len + 8 ] = CMD_END;

	LOG_DEBUG(TAG, "Send to web all data ! |||| "); 
	for(i = 0 ; i <  crc_data_len + 9 ; i ++)
		printf("%02x ",Send_Web_All_Data.Buf_Send[i]);
	printf("\r\n");
	LOG_DEBUG(TAG, "Send to web sending\r\n");

	pthread_mutex_lock(&pthread_mutex_ttys2_this);
	write(fd_ttys2, Send_Web_All_Data.Buf_Send,  crc_data_len + 9 ); //考虑数据中存在/0  长度会发生改变
	pthread_mutex_unlock(&pthread_mutex_ttys2_this);
	LOG_DEBUG(TAG, "Send to web send end\r\n"); 
	return 1;
}





xmlXPathObjectPtr getNodeset(xmlDocPtr pdoc,const xmlChar *xpath)
{
	xmlXPathContextPtr context=NULL;//XPath上下文指针
	xmlXPathObjectPtr result=NULL; //XPath结果指针
	context = xmlXPathNewContext(pdoc);

	if(pdoc==NULL){
		printf("pdoc is NULL\n");
		return NULL;
	}

	if(xpath){
		if (context == NULL) {
			printf("context is NULL\n");
			return NULL;
		}

		result = xmlXPathEvalExpression(xpath, context);
		xmlXPathFreeContext(context); //释放上下文指针
		if (result == NULL) {
			printf("xmlXPathEvalExpression return NULL\n");
			return NULL;
		}

		if (xmlXPathNodeSetIsEmpty(result->nodesetval)) {
			xmlXPathFreeObject(result);
			printf("nodeset is empty\n");
			return NULL;
		}
	}

	return result;
}
// todo 
int function_Cmd_Del_Dpcontrol_V3(char buf[],  int len)
{


	char * fseek_local = NULL;
	char *temp_buf1 = NULL, *temp_buf2 =NULL;
	xmlDocPtr pdoc = NULL;
	xmlNodePtr proot = NULL;
	int i = 0, j = 0, flag_del_dev_ok = 0;
	int controllerid = 0, devid = 0;
	xmlXPathObjectPtr result;
	cJSON *jsontemp = NULL, *jsonLast = NULL;

	int data_buf[16];
	xmlChar *value;

	char flag_exist_item = 0;
	for(i = 0; i< ((buf[4] << 8) + buf[5]); i++)
	{
		Send_Web_All_Data.buf_all_data.Buf_Send_WebData[i] = buf[6 + i];
		printf("%c ", Send_Web_All_Data.buf_all_data.Buf_Send_WebData[i]);
	}

	LOG_DEBUG(TAG, "function_Cmd_Set_Dpcontrol_V3 xml len = %d\r\n", (buf[4] << 8 ) + buf[5]);
	LOG_DEBUG(TAG, "xml before:%s\r\n", Send_Web_All_Data.buf_all_data.Buf_Send_WebData);

	if((fseek_local = strstr(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, "gb2312")) != NULL)
	{	
		*fseek_local = 'U';
		*(fseek_local + 1) = 'T';
		*(fseek_local + 2) = 'F';
		*(fseek_local + 3) = '-';
		*(fseek_local + 4) = '8';
		*(fseek_local + 5) = '\"';
		*(fseek_local + 6) = ' ';	
	}else{
		return ERROR_DEL_NOT_FOUND_CTRLID; 
	}

	LOG_DEBUG(TAG, "xml after :%s\r\n", Send_Web_All_Data.buf_all_data.Buf_Send_WebData);

	pdoc = xmlParseMemory(Send_Web_All_Data.buf_all_data.Buf_Send_WebData,( buf[4] << 8 ) + buf[5]);

	xmlKeepBlanksDefault(0);//必须加上，防止程序把元素前后的空白文本符号当作一个node

	proot = xmlDocGetRootElement (pdoc);

	if (proot == NULL)
	{
		printf("error: file is empty!\n");
		return ERROR_DEL_NOT_FOUND_CTRLID ;
	}

	xmlChar *xpath = BAD_CAST("//dev"); 
	result = getNodeset(pdoc, xpath); 
	if (result == NULL)
	{
		LOG_ERROR(TAG,"result is NULL\n");
		return ERROR_DEL_NOT_FOUND_CTRLID ;
	}

	if(result)
	{
		xmlNodeSetPtr nodeset = result->nodesetval; 
		xmlNodePtr cur;

		LOG_DEBUG(TAG, "nodenumber: %d\r\n",(nodeset->nodeNr));
		for (i=0; i < (nodeset->nodeNr); i++)
		{
			cur = nodeset->nodeTab[i];
			cur = cur->xmlChildrenNode;

			while (cur != NULL)
			{	
				LOG_DEBUG(TAG, "%s\r\n", cur->name);

				if (!xmlStrcmp(cur->name, BAD_CAST("d"))) {

					value = xmlGetProp(cur,"a6");
					printf("a6-->%d\r\n", (controllerid = atoi(value)));
					xmlFree(value);


					value = xmlGetProp(cur,"id");
					printf("id-->%d\r\n", (devid = atoi(value)));
					xmlFree(value);

					break;
				}
				cur = cur->next;
			}	
		}

		LOG_DEBUG(TAG, "xmlXPathFreeObject\r\n");
		xmlXPathFreeObject(result);
		xmlFreeDoc (pdoc);
		LOG_DEBUG(TAG, "xmlFreeDoc\r\n");
		xmlCleanupParser ();
		LOG_DEBUG(TAG, "xmlCleanupParser\r\n");
		xmlMemoryDump ();
		LOG_DEBUG(TAG, "xmlMemoryDump\r\n");

		jsontemp = cJSON_GetObjectItem(jsonroot, "controller");

		if(jsontemp != NULL){
			for(i = 0; i < cJSON_GetArraySize(jsontemp); i++)
			{
				if(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "conid") != NULL)
				{
					if(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "conid")->valueint == controllerid)
					{

						jsonLast = cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i), "dev");
						if(jsonLast != NULL)
						{
							for(j = 0; j < cJSON_GetArraySize(jsonLast); j++)
							{
								if(cJSON_GetObjectItem(cJSON_GetArrayItem(jsonLast, j), "devid") != NULL)
								{

									if(cJSON_GetObjectItem(cJSON_GetArrayItem(jsonLast, j), "devid")->valueint == devid)
									{
										cJSON_DeleteItemFromArray(jsonLast, j);

										SaveJsonToFile(jsonroot,JSON_CONFIG_FILE);
										flag_del_dev_ok = 1;
									}
								}	
							}
						}
					}
				}
			}
		}

	}
	if(flag_del_dev_ok != 1)
		return ERROR_DEL_NOT_FOUND_CTRLID;
	else

	return ERROR_SUCCESS; 
}




int function_Cmd_Set_Dpcontrol_V3(char buf[],int len, int *controllerid, int *devid)
{

	char * fseek_local = NULL;
	char *temp_buf1 = NULL, *temp_buf2 =NULL;
	xmlDocPtr pdoc = NULL;
	xmlNodePtr proot = NULL;
	int i = 0, j = 0;
	xmlXPathObjectPtr result;
	cJSON *jsontemp = NULL, *jsonLast = NULL, *jsontempmore =  NULL, *jsontempmoremore = NULL;
	int data_buf[16];
	xmlChar *value;

	char flag_exist_item = 0;
	for(i = 0; i< ((buf[4] << 8) + buf[5]); i++)
	{
		Send_Web_All_Data.buf_all_data.Buf_Send_WebData[i] = buf[6 + i];
		printf("%c ", Send_Web_All_Data.buf_all_data.Buf_Send_WebData[i]);
	}
	//strncat(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, &(buf[6]), (buf[4]<<8 + buf[5]));


	LOG_DEBUG(TAG, "function_Cmd_Set_Dpcontrol_V3 xml len = %d\r\n", (buf[4] << 8 ) + buf[5]);
	LOG_DEBUG(TAG, "xml before:%s\r\n", Send_Web_All_Data.buf_all_data.Buf_Send_WebData);

	if((fseek_local = strstr(Send_Web_All_Data.buf_all_data.Buf_Send_WebData, "gb2312")) != NULL)
	{	
		*fseek_local = 'U';
		*(fseek_local + 1) = 'T';
		*(fseek_local + 2) = 'F';
		*(fseek_local + 3) = '-';
		*(fseek_local + 4) = '8';
		*(fseek_local + 5) = '\"';
		*(fseek_local + 6) = ' ';	
	}else{
		return ERROR_NOT_XML; 
	}

	LOG_DEBUG(TAG, "xml after :%s\r\n", Send_Web_All_Data.buf_all_data.Buf_Send_WebData);

	pdoc = xmlParseMemory(Send_Web_All_Data.buf_all_data.Buf_Send_WebData,( buf[4] << 8 ) + buf[5]);

	xmlKeepBlanksDefault(0);//必须加上，防止程序把元素前后的空白文本符号当作一个node

	proot = xmlDocGetRootElement (pdoc);

	if (proot == NULL)
	{
		printf("error: file is empty!\n");
		return ERROR_NOT_FOUND_NODE;
	}

	xmlChar *xpath = BAD_CAST("//dev"); 
	result = getNodeset(pdoc, xpath); 
	if (result == NULL)
	{
		LOG_ERROR(TAG,"result is NULL\n");
		return 0;
	}

	if(result)
	{
		xmlNodeSetPtr nodeset = result->nodesetval; 
		xmlNodePtr cur;

		LOG_DEBUG(TAG, "nodenumber: %d\r\n",(nodeset->nodeNr));
		for (i=0; i < (nodeset->nodeNr); i++)
		{
			cur = nodeset->nodeTab[i];
			cur = cur->xmlChildrenNode;

			while (cur != NULL)
			{	
				LOG_DEBUG(TAG, "%s\r\n", cur->name);

				if (!xmlStrcmp(cur->name, BAD_CAST("d"))) {

					jsonLast = cJSON_CreateObject();

					value = xmlGetProp(cur,"a6");
					printf("a6-->%d\r\n", atoi(value));
					cJSON_AddNumberToObject(jsonLast, "a6", atoi(value));

					*controllerid = atoi(value);
					xmlFree(value);

					value = xmlGetProp(cur,"id");
					printf("id-->%d\r\n", atoi(value));
					cJSON_AddNumberToObject(jsonLast, "devid", atoi(value));
					*devid = atoi(value);
					xmlFree(value);

					value = xmlGetProp(cur,"type");
					printf("type-->%d\r\n", atoi(value));
					cJSON_AddNumberToObject(jsonLast, "type", atoi(value));
					xmlFree(value);

					value = xmlGetProp(cur,"relay");
					printf("-->%d\r\n", atoi(value));
					temp_buf2 = value;
					memset(data_buf, 0 ,sizeof(data_buf));
					i = 0;
					data_buf[i++] = atoi(value);
					while(temp_buf1 = strstr(temp_buf2, ","))
					{
						printf("-->%d\r\n", atoi(&(temp_buf1[1])));
						data_buf[i++] = atoi(&(temp_buf1[1]));
						temp_buf2 =&( temp_buf1[1]);
					}
					cJSON_AddItemToObject(jsonLast,"relay", cJSON_CreateIntArray(data_buf, i));
					// 		cJSON_AddItemToObject(jsonLast,"relay", cJSON_CreateIntArray(data_buf,strlen(data_buf)));
					xmlFree(value);

					value = xmlGetProp(cur,"hgq");
					if(value !=  NULL) // all should add if NULL
					{
						printf("-->%d\r\n", atoi(value));
						temp_buf2 = value;
						memset(data_buf, 0 ,sizeof(data_buf));
						i = 0;
						data_buf[i++] = atoi(value);
						while(temp_buf1 = strstr(temp_buf2, ","))
						{
							printf("-->%d\r\n", atoi(&(temp_buf1[1])));
							data_buf[i++] = atoi(&(temp_buf1[1]));
							temp_buf2 =&( temp_buf1[1]);
						}

						cJSON_AddItemToObject(jsonLast,"hgq", cJSON_CreateIntArray(data_buf, i ));
						xmlFree(value);
					}
					value = xmlGetProp(cur,"glsen");
					if(value != NULL)
					{
						cJSON_AddNumberToObject(jsonLast, "glsen", atoi(value));
						xmlFree(value);
					}
					value = xmlGetProp(cur,"glcollector");
					if(value != NULL)
					{
						cJSON_AddNumberToObject(jsonLast, "glcollector", atoi(value));
						xmlFree(value);
					}
					value = xmlGetProp(cur,"acon");
					if(value != NULL)
					{
						cJSON_AddNumberToObject(jsonLast, "acon", atoi(value));
						xmlFree(value);
					}
					value = xmlGetProp(cur,"re1");
					if(value != NULL)
					{
						cJSON_AddStringToObject(jsonLast, "re1", value);
						xmlFree(value);
					}
					value = xmlGetProp(cur,"re2");
					if(value != NULL)
					{
						cJSON_AddStringToObject(jsonLast, "re2", (value));
						xmlFree(value);
					}
					value = xmlGetProp(cur,"st1");
					if(value != NULL)
					{
						cJSON_AddStringToObject(jsonLast, "st1", value);
						xmlFree(value);
					}
					value = xmlGetProp(cur,"et1");
					if(value != NULL)
					{
						cJSON_AddStringToObject(jsonLast, "et1", (value));
						xmlFree(value);
					}

					value = xmlGetProp(cur,"st2");
					if(value != NULL)
					{
						cJSON_AddStringToObject(jsonLast, "st2", value);
						xmlFree(value);
					}
					value = xmlGetProp(cur,"et2");
					if(value != NULL)
					{
						cJSON_AddStringToObject(jsonLast, "et2", (value));
						xmlFree(value);
					}

					value = xmlGetProp(cur,"st3");
					if(value != NULL)
					{
						cJSON_AddStringToObject(jsonLast, "st3", value);
						xmlFree(value);
					}
					value = xmlGetProp(cur,"et3");
					if(value != NULL)
					{
						cJSON_AddStringToObject(jsonLast, "et3", (value));
						xmlFree(value);
					}

					LOG_DEBUG(TAG, "jsonLast -->\r\n%s\r\n", cJSON_Print(jsonLast));
					break;
				}

				cur = cur->next;
			}
			
		}

		LOG_DEBUG(TAG, "xmlXPathFreeObject\r\n");
		xmlXPathFreeObject(result);
		xmlFreeDoc (pdoc);
		LOG_DEBUG(TAG, "xmlFreeDoc\r\n");
		xmlCleanupParser ();
		LOG_DEBUG(TAG, "xmlCleanupParser\r\n");
		xmlMemoryDump ();
		LOG_DEBUG(TAG, "xmlMemoryDump\r\n");

		jsontemp = cJSON_GetObjectItem(jsonroot,"controller");
		LOG_DEBUG(TAG, "jsontemp size = %d\r\n", cJSON_GetArraySize(jsontemp));

		if(jsontemp != NULL)
		{
			if(cJSON_GetArraySize(jsontemp) == 0)
			{
				jsontempmore = cJSON_CreateObject();

				cJSON_AddNumberToObject(jsontempmore, "conid", cJSON_GetObjectItem(jsonLast,"a6")->valueint );
				cJSON_AddItemToObject(jsontempmore, "dev", (jsontempmoremore = cJSON_CreateArray()));
				cJSON_AddItemToArray(jsontempmoremore, jsonLast);

				cJSON_AddItemToArray(jsontemp, jsontempmore);
				SaveJsonToFile(jsonroot,JSON_CONFIG_FILE);
			}
			for(i = 0; i < cJSON_GetArraySize(jsontemp); i++)
			{
				if(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"conid") != NULL)
				{
					LOG_DEBUG(TAG, "i = %d conid = %d\r\n", i, cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"conid")->valueint );

					if(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"conid")->valueint == cJSON_GetObjectItem(jsonLast,"a6")->valueint)
					{
						LOG_DEBUG(TAG, "conid is exist\r\n");
						if(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"dev") != NULL)
						{
							LOG_DEBUG(TAG, "dev is exist\r\n");
							for(j = 0; j < cJSON_GetArraySize(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"dev")); j++)
							{
								jsontempmore = cJSON_GetArrayItem(cJSON_GetObjectItem( cJSON_GetArrayItem(jsontemp,i),"dev"),j);
								if(cJSON_GetObjectItem(jsontempmore,"devid") != NULL)
									if(cJSON_GetObjectItem(jsontempmore, "devid")->valueint == cJSON_GetObjectItem(jsonLast, "devid")->valueint)
							{	
								cJSON_ReplaceItemInArray(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"dev"), j, jsonLast);
								flag_exist_item = 1;
								break;
							}
						}

						if( flag_exist_item != 1)
							cJSON_AddItemToArray(cJSON_GetObjectItem(cJSON_GetArrayItem(jsontemp, i),"dev"), jsonLast);

							SaveJsonToFile(jsonroot,JSON_CONFIG_FILE);
						}else
						{

							cJSON_AddItemToObject(cJSON_GetArrayItem(jsontemp, i), "dev", (jsontempmoremore = cJSON_CreateArray()));
							cJSON_AddItemToArray(jsontempmoremore, jsonLast);
							SaveJsonToFile(jsonroot,JSON_CONFIG_FILE);

						}
						// todo save
					}
				}else
				{
				
				
				}

				if(flag_exist_item == 1)
					break;
			}
		}
	}
		return ERROR_SUCCESS; 
}


void send_data_to_web_on_time(void)
{

	int datalen = 0, i = 0,temp = 0;
	char buf_all_data[512];
	char buf[500];
	LOG_DEBUG(TAG, "send_data_to_web_on_time\r\n");

	memset(buf_all_data, 0, 512);

	buf_all_data[0] = 0xa5;
	buf_all_data[1] = 0x5a;
	buf_all_data[2] = 0x01;   // todo

	memset(buf, 0, 500);
	if(send_data_to_web_on_time_use_json_transformer(buf, &datalen) == 1)
	{
		buf_all_data[3] = CMD_READ_HUGANQI;
		buf_all_data[4] = datalen / 256;
		buf_all_data[5] = datalen % 256;


		LOG_DEBUG(TAG, "CRC16_Change before datalen = %d !\r\n", datalen); 
		for(i = 0; i < datalen; i ++)
		{
			buf_all_data[i + 6] = buf[i];
		}

		LOG_DEBUG(TAG, "CRC16_Change !\r\n"); 
		temp = CRC16_Change(buf_all_data, datalen  + 6 );
		buf_all_data[datalen + 6 ] = (temp &0x0000ff00 )>>8;
		buf_all_data[datalen + 7 ] = (temp &0x000000ff);
		buf_all_data[datalen + 8 ] = 0xbf;

		LOG_DEBUG(TAG, "send_data_to_web_on_time_use_json_transformer  !\r\n"); 
		for(i = 0 ; i <  datalen + 9 ; i ++)
			printf("%02x ",buf_all_data[i]);
		printf("\r\n");
		LOG_DEBUG(TAG, "Send to web sending\r\n"); 
		
		pthread_mutex_lock(&pthread_mutex_ttys2_this);
		write(fd_ttys2, buf_all_data,  datalen + 9 ); //考虑数据中存在/0  长度会发生改变
		pthread_mutex_unlock(&pthread_mutex_ttys2_this);
		LOG_DEBUG(TAG, "Send to web send end\r\n"); 

	}

	datalen  = 0;
	memset(buf_all_data, 0, 512);

	buf_all_data[0] = 0xa5;
	buf_all_data[1] = 0x5a;
	buf_all_data[2] = 0x01;   // todo

	memset(buf, 0, 500);
	if(send_data_to_web_on_time_use_json_collector(buf, &datalen) == 1)
	{
		buf_all_data[3] = CMD_READ_CAIJIQI;

		buf_all_data[4] = datalen / 256;
		buf_all_data[5] = datalen % 256;

		LOG_DEBUG(TAG, "CRC16_Change before datalen = %d !\r\n", datalen); 
		for(i = 0; i < datalen; i ++)
		{
			buf_all_data[i + 4] = buf[i];
		}

		LOG_DEBUG(TAG, "CRC16_Change !\r\n"); 
		temp = CRC16_Change(buf_all_data, datalen  + 6 );
		buf_all_data[datalen + 6 ] = (temp &0x0000ff00 )>>8;
		buf_all_data[datalen + 7 ] = (temp &0x000000ff);
		buf_all_data[datalen + 8 ] = 0xbf;

		LOG_DEBUG(TAG, "send_data_to_web_on_time_use_json_collector !\r\n"); 
		LOG_DEBUG(TAG, "Send to web all data !\r\n"); 
		for(i = 0 ; i <  datalen + 9 ; i ++)
			printf("%02x ",buf_all_data[i]);
		printf("\r\n");
		LOG_DEBUG(TAG, "Send to web sending\r\n"); 
		pthread_mutex_lock(&pthread_mutex_ttys2_this);
		write(fd_ttys2, buf_all_data,  datalen + 9 ); //考虑数据中存在/0  长度会发生改变
		pthread_mutex_unlock(&pthread_mutex_ttys2_this);
		LOG_DEBUG(TAG, "Send to web send end \r\n"); 

	}


}

int Task_TTYS2_Dtu_Web(void)
{
	int ret = 0;
	pthread_t tid;

	ret = pthread_create(&tid, NULL, thread_Task_TTYS2_Dtu_Web, NULL);
	if (ret != 0) {
		LOG_ERROR(TAG, "pthread_create error : %d\n", ret);
		ret = -1;
	}
	//	pthread_join(tid,NULL);
	LOG_DEBUG(TAG, "Task_TTYS2_Dtu_Web  start\n");

	return ret;
}

