#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>
#include "log.h"
#include "task_ttys1_zigbee_dev.h"
#include "json_function.h"
#include "cJSON.h"
enum{
	SEEK = 1,
	ADD,
	DEL,
	CHANGE,
	QUIT,
};

unsigned int serialnumber = 1;
unsigned int serialmax = 0, serialmin = 0;
static const char TAG[] = "Sqlite";
sqlite3 *db;
int sqlitefd;
char *err;
char all[320] = {'\0'}, time_all[24] = {'\0'}, data[300] = {'\0'};
char mark[350] = {'\0'};

int callback(void *para, int f_num, char **f_value, char **f_name)
{
	int i = 0, j = 0;

	for(i = 0; i < f_num; i++)	
	{
		LOG_DEBUG(TAG,"%s: %s\n",f_name[i], f_value[i]);

		//LOG_DEBUG(TAG,"para:%s\n",(char *)para);

		if((para !=NULL)) //  exec forth para
		{
			LOG_DEBUG(TAG,"para[0]:%c\n",((char *)para)[0]);
			if(strcmp(f_name[i], "serial") == 0)
			{
				if((strncmp(para,"9", 1) == 0))
				{
					serialmax = atoi(f_value[i]);
					LOG_DEBUG(TAG,"serialmax == %d\r\n", serialmax);
				}
				else if((strncmp(para,"1", 1) == 0))
				{
					serialmin = atoi(f_value[i]);
					LOG_DEBUG(TAG,"serialmax == %d\r\n", serialmin);
				}
			}
			bzero(para, 1);
		}
		else if(strcmp(f_name[i], "serial") == 0)
		{
			LOG_DEBUG(TAG,"serial == %d\r\n", atoi(f_value[i]));
		}
		else if(strcmp(f_name[i], "time") == 0)
		{
			sprintf(time_all, "%s", f_value[i]);
			LOG_DEBUG(TAG,"time == %s\r\n", (f_value[i]));
		}
		else if(strcmp(f_name[i], "data") == 0)
		{
			LOG_DEBUG(TAG,"data == %s\r\n", (f_value[i]));
			sprintf(data, "%s", f_value[i]);

		}
		else if(strstr(f_name[i], "count") != NULL)
		{

			serialnumber = atoi(f_value[i]);
			LOG_DEBUG(TAG,"serialnumber == %d\r\n",serialnumber);
		}

	}
	//	strcpy(back, *f_value);
	return 0;
}

#define SQLITE_FILE "./data.db"
int create_dic(int sqlitefd, sqlite3 **db)
{
	// tail -1 | od -c  test
	char buf;
	char buf_temp[16];
	sqlitefd = sqlite3_open(SQLITE_FILE, db);
	if(sqlitefd != 0)
	{
		LOG_DEBUG(TAG, "sqlitefd open error\n");
		return 0;
	}
	LOG_DEBUG(TAG, "sqlitefd open ok\n");
	
	sqlite3_exec(*db, "create table collectordata (serial integer, time text,  data text);", 
			NULL, NULL, &err);
	if(err != NULL)
	{
		LOG_ERROR(TAG, "err:%s\r\n", err);
		/*
		   LOG_DEBUG(TAG,"delect the table (y or n)?\n");
		   buf = fgetc(stdin);
		   if(buf == 'y')
		   {
		   sqlite3_close(*db);
		   sprintf(buf_temp, "rm -rf %s", SQLITE_FILE);
		   LOG_DEBUG(TAG,"%s\r\n", buf_temp);
		   system(buf_temp);
		   sqlitefd = sqlite3_open(SQLITE_FILE, db);

		   if(sqlitefd != 0)
		   {
		   puts("sqlitefd error\n");
		   return 0;
		   }
		   sqlite3_exec(*db, "create table collectordata (serial integer, time text,  data text);", 
		   NULL, NULL, &err);
		   }
		   else
		//	exit(-1); 		//
		return 0;
		*/
	}

	sqlite3_exec(*db, "create table transformerdata (serial integer, time text, data text);", 
			NULL, NULL, &err);
	if(err != NULL)
	{
		LOG_ERROR(TAG, "err:%s\r\n", err);
	}
}

char get_now_time(char *time_buf,int len)
{
	time_t timer;
	struct tm  *time_nowtm;
	int buf_int[6];
	int i = 0;

	time(&timer);
	time_nowtm = localtime(&timer);

	memset(time_buf, 0, len);
	if(time_nowtm == NULL)
		return 0;
	memset(buf_int, 0, sizeof(buf_int)/ sizeof(int));
	buf_int[0] = ( time_nowtm->tm_year + 1900 );
	buf_int[1] = ( time_nowtm->tm_mon + 1 );
	buf_int[2] = ( time_nowtm->tm_mday );
	buf_int[3] = ( time_nowtm->tm_hour );
	buf_int[4] = ( time_nowtm->tm_min );
	buf_int[5] = ( time_nowtm->tm_sec );
	sprintf(time_buf,"%d:%d:%d:%d:%d:%d", buf_int[0], buf_int[1],buf_int[2], buf_int[3],buf_int[4],buf_int[5]);

	LOG_DEBUG(TAG,"get_now_time = %s\r\n",time_buf);
	return 1;
}

char get_data_from_sqlite_to_web(int fd)
{
	char temp_Data_get_max[1] = {'9'};
	char temp_Data_get_min[1] = {'1'};
	static  unsigned  int  post_serial_change = 0;
	static unsigned char first_enter_get_start = 0;
	char buf_temp[64];
	char *buf_all_data_to_web;
	cJSON *json_get_data, *json_send_web_data, *json_temp;
	FILE *fp;
	int data_len = 0, temp = 0;
	unsigned char i = 0, j = 0;
	LOG_DEBUG(TAG, "get_data_from_sqlite_to_web ...\r\n");
	sqlite3_exec(db, "select * from transformerdata order by serial desc limit 0,1;", callback, temp_Data_get_max, &err);
	if(err != NULL)
	{
		LOG_ERROR(TAG, "sqlite3_exec error:%s\r\n",err);
		return -1;
	}

	LOG_DEBUG(TAG, "get_data_from_sqlite_to_web  serialmax: %d\r\n", serialmax);

	usleep(1000 * 10);
	sqlite3_exec(db, "select * from transformerdata order by serial asc limit 0,1;", callback, temp_Data_get_min, &err);
	if(err != NULL)
	{
		LOG_ERROR(TAG, "sqlite3_exec error:%s\r\n",err);
		return -1;
	}
	LOG_DEBUG(TAG, "get_data_from_sqlite_to_web  serialmin: %d\r\n", serialmin);

	if(serialmax ==  serialmin)
	{
		sqlite3_exec(db, "select count (*) from collectordata;", 
				callback, NULL, &err);
		if(err != NULL)
		{
			puts(err);
		}

		LOG_DEBUG(TAG, "get_sqlite3_serial all number = %d\r\n", serialnumber);
		if(serialnumber == 0)
		{
			serialnumber = 0;
			post_serial_change = 0;
			return -1;
		}
	}

	LOG_DEBUG(TAG, " post_serial_change: %d\r\n", post_serial_change);
	memset(buf_temp, 0,  sizeof(buf_temp));
	sprintf(buf_temp, "select * from transformerdata where serial = %d;", post_serial_change);
	sqlite3_exec(db, buf_temp, callback,  NULL, &err);
	if(err != NULL)
	{
		LOG_ERROR(TAG, "sqlite3_exec error:%s\r\n",err);
	}
	LOG_DEBUG(TAG, "transformerdata --> time :%s, data :%s\r\n", time_all, data);
	// try to cut and Send 

	json_get_data = cJSON_Parse(data);
	LOG_DEBUG(TAG, "json_get_data = %s\r\n", cJSON_Print(json_get_data));

	buf_all_data_to_web = (char *)malloc(1000);
	memset(buf_all_data_to_web, 0, 1000);
	buf_all_data_to_web[0] = 0xa5;
	buf_all_data_to_web[1] = 0x5a;
	buf_all_data_to_web[2] = 0x01;
	buf_all_data_to_web[3] = CMD_GET_SQLITE_DATA; 

	if(json_get_data !=  NULL)
		for(i = 0; i < cJSON_GetArraySize(json_get_data); i++)
		{

			json_send_web_data = cJSON_CreateObject();
			cJSON_AddStringToObject(json_send_web_data, "time", time_all);
			json_temp = cJSON_GetArrayItem(json_get_data, i);
			cJSON_AddItemToObject(json_send_web_data, "data", json_temp);

			LOG_ERROR(TAG, "json_send_web_data1 :\r\n%s\r\n", cJSON_PrintUnformatted(json_send_web_data));
	
			data_len = strlen(cJSON_PrintUnformatted(json_send_web_data));

			LOG_DEBUG(TAG, "data_len = %d\r\n", data_len);
			buf_all_data_to_web[4] = data_len / 256;
			buf_all_data_to_web[5] = data_len % 256;

			strcat(buf_all_data_to_web,cJSON_PrintUnformatted(json_send_web_data));

			temp = CRC16_Change(buf_all_data_to_web, data_len  + 6 );
			buf_all_data_to_web[data_len + 6 ] = (temp &0x0000ff00 )>>8;
			buf_all_data_to_web[data_len + 7 ] = (temp &0x000000ff);
			buf_all_data_to_web[data_len + 8 ] = 0xbf;

			for(j = 0; j < (data_len + 9); j ++)
				printf("%02x ",buf_all_data_to_web[j]);

			printf("\r\n");
			write(fd, buf_all_data_to_web, (data_len + 9));
			// todo
		}

	sprintf(buf_temp, "delete  from transformerdata where serial = %d;", post_serial_change);
	sqlite3_exec(db, buf_temp, NULL,  NULL, &err);
	if(err != NULL)
	{
		LOG_ERROR(TAG, "sqlite3_exec error:%s\r\n",err);
	}




	memset(buf_temp, 0,  sizeof(buf_temp));
	sprintf(buf_temp, "select * from collectordata where serial = %d;", post_serial_change);
	sqlite3_exec(db, buf_temp, callback,  NULL, &err);
	if(err != NULL)
	{
		LOG_ERROR(TAG, "sqlite3_exec error:%s\r\n",err);
	}
	LOG_DEBUG(TAG, "collectordata -->time :%s, data :%s\r\n", time_all, data);

	json_get_data = cJSON_Parse(data);
	LOG_DEBUG(TAG, "json_get_data = %s\r\n", cJSON_Print(json_get_data));

	buf_all_data_to_web[0] = 0xa5;
	buf_all_data_to_web[1] = 0x5a;
	buf_all_data_to_web[2] = 0x01;
	buf_all_data_to_web[3] = CMD_GET_SQLITE_DATA; 
	if(json_get_data !=  NULL)
		for(i = 0; i < cJSON_GetArraySize(json_get_data); i++)
		{

			json_send_web_data = cJSON_CreateObject();
			cJSON_AddStringToObject(json_send_web_data, "time", time_all);
			json_temp = cJSON_GetArrayItem(json_get_data, i);
			cJSON_AddItemToObject(json_send_web_data, "data", json_temp);

			LOG_ERROR(TAG, "json_send_web_data2 :\r\n%s\r\n", cJSON_PrintUnformatted(json_send_web_data));
			
			data_len = strlen(cJSON_PrintUnformatted(json_send_web_data));
			LOG_DEBUG(TAG, "data_len = %d\r\n", data_len);
			buf_all_data_to_web[4] = data_len / 256;
			buf_all_data_to_web[5] = data_len % 256;

			strcat(buf_all_data_to_web,cJSON_PrintUnformatted(json_send_web_data));

			temp = CRC16_Change(buf_all_data_to_web, data_len  + 6 );
			buf_all_data_to_web[data_len + 6 ] = (temp &0x0000ff00 )>>8;
			buf_all_data_to_web[data_len + 7 ] = (temp &0x000000ff);
			buf_all_data_to_web[data_len + 8 ] = 0xbf;


			for(j = 0; j < (data_len + 9); j ++)
				printf("%02x ",buf_all_data_to_web[j]);

			printf("\r\n");
			write(fd, buf_all_data_to_web, (data_len + 9));
			// send to web  
			// todo
		}

	sprintf(buf_temp, "delete  from collectordata where serial = %d;", post_serial_change);
	sqlite3_exec(db, buf_temp, NULL,  NULL, &err);
	if(err != NULL)
	{
		LOG_ERROR(TAG, "sqlite3_exec error:%s\r\n",err);
	}

	if(serialmax <  19999)
	{
		post_serial_change ++;

	}else
	{
		if(first_enter_get_start == 0)
		{
			first_enter_get_start = 1;
			fp = fopen("serialnumber.txt", "rb+");
			if(!fp)
			{
				LOG_ERROR(TAG,"serialnumber open fail\r\n");
			}else{
				fread(data, 1, 8, fp);
				post_serial_change = atoi(data) + 1;		
			}
		}else
			post_serial_change = 0;

	}

	return 1;

}

char save_data_to_sqlite(void)
{

	FILE* fp;
	cJSON *JsonRoot1, *json_trans, *json_coll;
	char *trans_point, *coll_point;
	char buf_system[32];

	char time_buf[24],buf_temp[512];

	LOG_DEBUG(TAG, "save_data_to_sqlite ...\r\n");
	if((JsonRoot1 =  GetJsonObject(SHARE_MEM_FIEL)) == NULL)
	{
		LOG_ERROR(TAG, "save_data_to_sqlite  error jsonroot is null");
		return -1;
	}else{

		json_trans = cJSON_GetObjectItem(JsonRoot1, "transformer");
		if(json_trans != NULL)
		{
			trans_point = cJSON_PrintUnformatted(json_trans); 
			LOG_DEBUG(TAG, "trans_point :%s\r\n", trans_point);
		}
		json_coll = cJSON_GetObjectItem(JsonRoot1, "collector");
		if(json_coll != NULL)
		{
			coll_point = cJSON_PrintUnformatted(json_coll); 
			LOG_DEBUG(TAG,"coll_point :%s\r\n", coll_point);
		}
	}

	get_now_time(time_buf, sizeof(time_buf));
	sprintf(buf_temp, "insert into  transformerdata values (%d, \'%s\', \'%s\');", serialnumber, time_buf, trans_point);
	printf("\r\n%s\r\n", buf_temp);
	sqlite3_exec(db, buf_temp, NULL, NULL, &err);
	if(err != NULL)
	{
		LOG_ERROR(TAG, "sqlite3_exec error:%s\r\n",err);
	}
	sprintf(buf_temp, "insert into   collectordata  values (%d, \'%s\', \'%s\');", serialnumber, time_buf, coll_point);
	printf("\r\n%s\r\n", buf_temp);
	sqlite3_exec(db, buf_temp, NULL, NULL, &err);
	if(err != NULL)
	{
		LOG_ERROR(TAG, "sqlite3_exec error:%s\r\n",err);
	}
	if(serialnumber ++ <= 20000)
	{
		memset(buf_system, 0, sizeof(buf_system));
		sprintf(buf_system, "echo '%d' > serialnumber.txt", serialnumber);
		system(buf_system);
	}else
		serialnumber = 0;
}
void create_sqlite3(void)
{
	create_dic(sqlitefd, &db);
}

void get_sqlite3_serial_max(void)
{
	FILE* fp;
	char data[8];
	sqlite3_exec(db, "select count (*) from collectordata;", 
			callback, NULL, &err);
	if(err != NULL)
	{
		puts(err);
	}

	LOG_DEBUG(TAG, "get_sqlite3_serial_max = %d\r\n", serialnumber);
	if(serialnumber >= 1999)
	{
		fp = fopen("serialnumber.txt", "rb+");
		if(!fp)
		{
			LOG_ERROR(TAG,"serialnumber open fail\r\n");
		}else{

			fread(data, 1, 8, fp);
			serialnumber = atoi(data);		
			LOG_DEBUG(TAG, "serialnumber from txt :%d\r\n", serialnumber);
		}
	}
}

/*
   int main(int argc, const char *argv[])
   {

   create_dic(sqlitefd, &db);

   menu_sqlite();
   sqlite3_close(db);

   return 0;
   }
   */
