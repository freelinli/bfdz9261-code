#include <stdio.h>
#include <string.h>
#include <sys/types.h>	       /* See NOTES */
#include <unistd.h>

#include "json_function.h"
#include <time.h>

#include "log.h"

#include <pthread.h>


static const char TAG[] = "Json_Function";


static pthread_mutex_t pthread_mutex_this = PTHREAD_MUTEX_INITIALIZER;


void file_json_decode(cJSON *json)
{
	cJSON * JsonOption2;
	cJSON * JsonNode3;
	cJSON * JsonData4;
	cJSON * JsonLast, *JsonLastLast;
	int size = 0, i = 0,j = 0, k = 0, arraysize1 = 0, arraysize2 = 0;

	if(json == NULL)
		return;

	// ---------------------------------------------------------------------//
	// configdata
	// ---------------------------------------------------------------------//

	LOG_DEBUG(TAG," config ---------------------------------------------------------------------\r\n");
	JsonOption2 = JsonNode3 = JsonData4 = NULL;

	JsonOption2 =  cJSON_GetObjectItem(json, "config");

	if(JsonOption2 != NULL)
	{
		LOG_DEBUG(TAG,"ver = %d\r\n", cJSON_GetObjectItem(JsonOption2,"ver")->valueint);

		LOG_DEBUG(TAG,"cver = %d\r\n", cJSON_GetObjectItem(JsonOption2,"cver")->valueint);

		JsonNode3 = cJSON_GetObjectItem(JsonOption2,"ip");


		size = cJSON_GetArraySize(JsonNode3);
		LOG_DEBUG(TAG,"ip =");
		for(i = 0; i < size; i ++ ){
			LOG_DEBUG(TAG,	"%d-", 
					cJSON_GetArrayItem(JsonNode3, i)->valueint);
		}	
		LOG_DEBUG(TAG,"\r\n");


		LOG_DEBUG(TAG,"port = %d\r\n", cJSON_GetObjectItem(JsonOption2,"port")->valueint);

		LOG_DEBUG(TAG,"zigbeeport = %s\r\n", cJSON_GetObjectItem(JsonOption2,"zigbeeport")->valuestring);
		LOG_DEBUG(TAG,"webport = %s\r\n", cJSON_GetObjectItem(JsonOption2,"webport")->valuestring);

		JsonNode3 = cJSON_GetObjectItem(JsonOption2,"datetime");
		if(JsonNode3 != NULL)
		{

			LOG_DEBUG(TAG,"year = %d\r\n", cJSON_GetObjectItem(JsonNode3,"year")->valueint);
			LOG_DEBUG(TAG,"mon = %d\r\n", cJSON_GetObjectItem(JsonNode3,"mon")->valueint);
			LOG_DEBUG(TAG,"day = %d\r\n", cJSON_GetObjectItem(JsonNode3,"day")->valueint);
			LOG_DEBUG(TAG,"hour = %d\r\n", cJSON_GetObjectItem(JsonNode3,"hour")->valueint);
			LOG_DEBUG(TAG,"minute = %d\r\n", cJSON_GetObjectItem(JsonNode3,"minute")->valueint);
			LOG_DEBUG(TAG,"second = %d\r\n", cJSON_GetObjectItem(JsonNode3,"second")->valueint);
		}
	}


	// ---------------------------------------------------------------------//
	// collector
	// ---------------------------------------------------------------------//

	LOG_DEBUG(TAG," collector---------------------------------------------------------------------\r\n");
	JsonOption2 = JsonNode3 = JsonData4 = NULL;
	JsonOption2 = cJSON_GetObjectItem(json,"collector");

	if(JsonOption2 != NULL)
	{
		size  = cJSON_GetArraySize(JsonOption2); 
		LOG_DEBUG(TAG,"collector addr size = %d\r\n", size);


		for(i = 0; i < size; i ++)
		{
			JsonNode3  = cJSON_GetArrayItem(JsonOption2, i);
			//		LOG_DEBUG(TAG,"node [%d]addr is  %s \r\n", i + 1, cJSON_Print(JsonNode3));

			if(JsonNode3 != NULL)
			{
				LOG_DEBUG(TAG,"a2 = %d\r\n", cJSON_GetObjectItem(JsonNode3,"a2")->valueint);
				LOG_DEBUG(TAG,"a4 = %d\r\n", cJSON_GetObjectItem(JsonNode3,"a4")->valueint);
				LOG_DEBUG(TAG,"a5 = %d\r\n", cJSON_GetObjectItem(JsonNode3,"a5")->valueint);
				LOG_DEBUG(TAG,"dc = %d\r\n", cJSON_GetObjectItem(JsonNode3,"dc")->valueint);
				LOG_DEBUG(TAG,"cs = %d\r\n", cJSON_GetObjectItem(JsonNode3,"cs")->valueint);
				LOG_DEBUG(TAG,"temps = %d\r\n", cJSON_GetObjectItem(JsonNode3,"temps")->valueint);
				LOG_DEBUG(TAG,"tempx = %d\r\n", cJSON_GetObjectItem(JsonNode3,"tempx")->valueint);

				LOG_DEBUG(TAG,"humiditys = %d\r\n", cJSON_GetObjectItem(JsonNode3,"humiditys")->valueint);
				LOG_DEBUG(TAG,"humidityx = %d\r\n", cJSON_GetObjectItem(JsonNode3,"humidityx")->valueint);
				LOG_DEBUG(TAG,"co2s = %d\r\n", cJSON_GetObjectItem(JsonNode3,"co2s")->valueint);
				LOG_DEBUG(TAG,"co2x = %d\r\n", cJSON_GetObjectItem(JsonNode3,"co2x")->valueint);

				LOG_DEBUG(TAG,"oilHumiditys = %d\r\n", cJSON_GetObjectItem(JsonNode3,"oilHumiditys")->valueint);
				LOG_DEBUG(TAG,"oilHumidityx = %d\r\n", cJSON_GetObjectItem(JsonNode3,"oilHumidityx")->valueint);


				JsonData4 =  cJSON_GetObjectItem(JsonNode3, "a6");
				if(JsonData4 !=  NULL)
				{

					arraysize1 = cJSON_GetArraySize(JsonData4);

					LOG_DEBUG(TAG,"a6 size = %d\r\n", arraysize1);
					for(j = 0; j < arraysize1; j ++)
					{
						LOG_DEBUG(TAG,"a6[%d] = %d\r\n", k + 1, cJSON_GetArrayItem(JsonData4,j)->valueint);
					}

				}
				JsonData4 =  cJSON_GetObjectItem(JsonNode3, "a7");
				if(JsonData4 !=  NULL)
				{

					arraysize1 = cJSON_GetArraySize(JsonData4);

					LOG_DEBUG(TAG,"a7 size = %d\r\n", arraysize1);
					for(j = 0; j < arraysize1; j ++)
					{
						LOG_DEBUG(TAG,"a7[%d] = %d\r\n", k + 1, cJSON_GetArrayItem(JsonData4,j)->valueint);

					}

				}



			}


		}

	}

	LOG_DEBUG(TAG," controller---------------------------------------------------------------------\r\n");
	// ---------------------------------------------------------------------//
	// controller
	// ---------------------------------------------------------------------//

	JsonOption2 = JsonNode3 = JsonData4 = NULL;
	JsonOption2 = cJSON_GetObjectItem(json,"controller");

	if(JsonOption2 != NULL)
	{
		size  = cJSON_GetArraySize(JsonOption2); 
		LOG_DEBUG(TAG,"controller addr size = %d\r\n", size);

		for(i = 0; i < size; i ++)
		{

			JsonNode3  = cJSON_GetArrayItem(JsonOption2, i);

			if(JsonNode3 !=  NULL)
			{
				//			LOG_DEBUG(TAG,"node [%d]addr is  %s \r\n", i + 1, cJSON_Print(JsonNode3));


				LOG_DEBUG(TAG,"conid = %s\r\n", cJSON_GetObjectItem(JsonNode3,"conid")->valuestring);

				JsonData4 = cJSON_GetObjectItem(JsonNode3, "dev");

				if(JsonData4 !=  NULL)
				{
					arraysize1 = cJSON_GetArraySize(JsonData4);

					LOG_DEBUG(TAG,"dev size = %d\r\n", arraysize1);
					for(j = 0; j < arraysize1; j ++)
					{

						JsonLast  = cJSON_GetArrayItem(JsonData4, j);


						if(JsonLast !=  NULL)
						{

							LOG_DEBUG(TAG,"devid = %d\r\n", cJSON_GetObjectItem(JsonLast,"devid")->valueint);
							LOG_DEBUG(TAG,"a6 = %s\r\n", cJSON_GetObjectItem(JsonLast,"a6")->valuestring);
							LOG_DEBUG(TAG,"glcollector = %d\r\n", cJSON_GetObjectItem(JsonLast,"glcollector")->valueint);
							LOG_DEBUG(TAG,"glsen = %d\r\n", cJSON_GetObjectItem(JsonLast,"glsen")->valueint);
							LOG_DEBUG(TAG,"acon = %d\r\n", cJSON_GetObjectItem(JsonLast,"acon")->valueint);

							LOG_DEBUG(TAG,"res1 = %s\r\n", cJSON_GetObjectItem(JsonLast,"re1")->valuestring);
							LOG_DEBUG(TAG,"res2 = %s\r\n", cJSON_GetObjectItem(JsonLast,"re2")->valuestring);
							LOG_DEBUG(TAG,"st1 = %s\r\n", cJSON_GetObjectItem(JsonLast,"st1")->valuestring);
							LOG_DEBUG(TAG,"et1 = %s\r\n", cJSON_GetObjectItem(JsonLast,"et1")->valuestring);
							LOG_DEBUG(TAG,"st2 = %s\r\n", cJSON_GetObjectItem(JsonLast,"st2")->valuestring);
							LOG_DEBUG(TAG,"et2 = %s\r\n", cJSON_GetObjectItem(JsonLast,"et2")->valuestring);
							LOG_DEBUG(TAG,"st3 = %s\r\n", cJSON_GetObjectItem(JsonLast,"st3")->valuestring);
							LOG_DEBUG(TAG,"et3 = %s\r\n", cJSON_GetObjectItem(JsonLast,"et3")->valuestring);
							LOG_DEBUG(TAG,"type = %d\r\n", cJSON_GetObjectItem(JsonLast,"type")->valueint);

							JsonLastLast = cJSON_GetObjectItem(JsonLast, "relay");

							arraysize2 = cJSON_GetArraySize(JsonLastLast);
							LOG_DEBUG(TAG,"relay size = %d\r\n", arraysize2);
							for(k = 0; k < arraysize2; k ++)
							{
								LOG_DEBUG(TAG,"relay[%d] = %d\r\n", k + 1, cJSON_GetArrayItem(JsonLastLast,k)->valueint);
							}


							JsonLastLast = cJSON_GetObjectItem(JsonLast, "hgq");

							arraysize2 = cJSON_GetArraySize(JsonLastLast);
							LOG_DEBUG(TAG,"hgq size = %d\r\n", arraysize2);
							for(k = 0; k < arraysize2; k ++)
							{
								LOG_DEBUG(TAG,"hgq[%d] = %d\r\n", k + 1, cJSON_GetArrayItem(JsonLastLast,k)->valueint);
							}

						}

					}

				}
			}
		}

	}

}

//1. 读取一个json文件，返回json结构链表，注意，这里返回值必须为cJSON*，具体原因看上一篇文章。另外关于json的介绍看，http://www.json.org/json-zh.html

#if 0

#else
cJSON* GetJsonObject(char* fileName)
{

	cJSON *json;
	long len;
	char* pContent;
	int tmp;
	FILE* fp = fopen(fileName, "rb+");
	if(!fp)
	{
		LOG_ERROR(TAG,"GetJsonObject open fail\r\n");

		return NULL;
	}
	fseek(fp,0,SEEK_END);
	len=ftell(fp);
//	LOG_DEBUG(TAG,"len = %ld\r\n",len);
	if(0 == len)
	{
		LOG_ERROR(TAG,"GetJsonObject  len  == 0\r\n");
		return NULL;
	}

	fseek(fp,0,SEEK_SET);
	pContent = (char*) malloc (sizeof(char)*len);
	tmp = fread(pContent,1,len,fp);

	//	LOG_DEBUG(TAG,"%s\r\n", pContent);
	fclose(fp);
	json=cJSON_Parse(pContent);
	if (!json)
	{
		LOG_ERROR(TAG,"cJSON_Parse error\r\n");
		free(pContent);
		return NULL;
	}

//	LOG_DEBUG(TAG,"cJSON_Parse return json \r\n");
	free(pContent);
	return json;
}

#endif

//2 读取cJSON索引为index的结点某个key值对应的value，索引从0开始


char  GetValueString(cJSON* json,int id, char* name, char* param)
{
	cJSON* node;
	node = cJSON_GetArrayItem(json,id);
	if(!node)
	{
		return 0;
	}
	sprintf(param, "%s", cJSON_GetObjectItem(node, name)->valuestring);
	return 1;
}


char  GetValueNumber(cJSON* json,int id, char* name, int *number)
{
	cJSON* node;
	char * p; 
	node = cJSON_GetArrayItem(json,id);

	if(!node)
	{
		LOG_ERROR(TAG,"cJSON_GetArrayItem error\r\n");
		return 0;
	}

	if( (node = cJSON_GetObjectItem(node, name)) != NULL)
	{
		*number = node->valueint;
		return 1;

	}else
	{
		LOG_ERROR(TAG,"cJSON_GetObjectItem error\r\n");
		return 0;
	}

}
char Create_Sharemem_Json(char *buff, int len)
{
	cJSON * JsonRoot1, *JsonOption2, *JsonNode3;
	int i = 0 ,j = 0;

	struct tm  *time_nowtm;
	int buf_int[128];
	char flag_exist_repalce = 0;

	struct{
		struct {
			int tm_sec;	   /* seconds */
			int tm_min;	   /* minutes */
			int tm_hour;	   /* hours */
			int tm_mday;	   /* day of the month */
			int tm_mon;	   /* month */
			int tm_year;	   /* year */
			int tm_wday;	   /* day of the week */
			int tm_yday;	   /* day in the year */
			int tm_isdst;	   /* daylight saving time */
		}tm;


	}time_now;
	time_t timer;
	char *buf = (char *)malloc(len);

	for(i = 0 ; i < len; i++)
	{
		buf[i] = buff[i];
	}
	if((buf[0] == 0xa5)&&(buf[1] == 0x5a) &&(buf[len - 1] == 0xbf))
	{

		time(&(timer));
		time_nowtm= localtime(&timer);

		memset(buf_int, 0, sizeof(buf_int));
		buf_int[0] = ( time_nowtm->tm_year + 1900 );
		buf_int[1] = ( time_nowtm->tm_mon + 1 );
		buf_int[2] = ( time_nowtm->tm_mday );
		buf_int[3] = ( time_nowtm->tm_hour );
		buf_int[4] = ( time_nowtm->tm_min );
		buf_int[5] = ( time_nowtm->tm_sec );

		if((JsonRoot1 = GetJsonObject(SHARE_MEM_FIEL)) == NULL)
		{
			JsonRoot1 = cJSON_CreateObject();
		}
		if(cJSON_GetObjectItem(JsonRoot1, "date") == NULL)
		{
			cJSON_AddItemToObject(JsonRoot1, "date", cJSON_CreateIntArray(buf_int,6));
		}else
		{
			cJSON_ReplaceItemInObject(JsonRoot1, "date", cJSON_CreateIntArray(buf_int,6));
		}

		JsonNode3 = cJSON_CreateObject();
		cJSON_AddNumberToObject(JsonNode3, "id", buf[2]);

		memset(buf_int, 0, sizeof(buf_int));
		for(i = 0; i < buf[4]; i++)
			buf_int[i] = buf[5 + i];
		cJSON_AddItemToObject(JsonNode3, "data", cJSON_CreateIntArray(buf_int, buf[4]));

		switch(buf[3])
		{
		case 3: // collector

			if(cJSON_GetObjectItem(JsonRoot1, "collector") == NULL)
			{
				JsonOption2 = cJSON_CreateArray();
				cJSON_AddItemToArray(JsonOption2, JsonNode3);
				cJSON_AddItemToObject(JsonRoot1, "collector", JsonOption2);
			}else
			{

				JsonOption2 = cJSON_GetObjectItem(JsonRoot1, "collector");

				for(i = 0; i < cJSON_GetArraySize(JsonOption2); i++)
				{

					if(cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "id")->valueint
							== cJSON_GetObjectItem(JsonNode3, "id")->valueint)
					{

						if(cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "data") != NULL)
							cJSON_ReplaceItemInObject(cJSON_GetArrayItem(JsonOption2, i), "data", cJSON_CreateIntArray(buf_int, buf[4]));
						else
							cJSON_AddItemToObject(cJSON_GetArrayItem(JsonOption2, i), "data", cJSON_CreateIntArray(buf_int, buf[4]));
						//cJSON_ReplaceItemInArray(JsonOption2, i, JsonNode3);
				
						flag_exist_repalce = 1;
						break;
					}
				}
			//	if(flag_exist_repalce == 0) // if flag_exist_repalce = 0 del or 
			//		cJSON_AddItemToArray(JsonOption2, JsonNode3);
			}

			break;

		case 5: // controller
			if(cJSON_GetObjectItem(JsonRoot1, "controller") == NULL)
			{

				JsonOption2 = cJSON_CreateArray();
				cJSON_AddItemToArray(JsonOption2, JsonNode3);
				cJSON_AddItemToObject(JsonRoot1, "controller", JsonOption2);
			}else
			{
				JsonOption2 = cJSON_GetObjectItem(JsonRoot1, "controller");
				for(i = 0; i < cJSON_GetArraySize(JsonOption2); i++)
				{
					if(cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2,i), "id")->valueint
							== cJSON_GetObjectItem(JsonNode3, "id")->valueint)
					{
						if(cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "data") != NULL)
							cJSON_ReplaceItemInObject(cJSON_GetArrayItem(JsonOption2, i), "data", cJSON_CreateIntArray(buf_int, buf[4]));
						else
							cJSON_AddItemToObject(cJSON_GetArrayItem(JsonOption2, i), "data", cJSON_CreateIntArray(buf_int, buf[4]));
				//		cJSON_ReplaceItemInArray(JsonOption2, i, JsonNode3);

						flag_exist_repalce = 1;
						break;
					}
				}
				if(flag_exist_repalce == 0)
					cJSON_AddItemToArray(JsonOption2, JsonNode3);

			}
			break;

		case 2: // transformer
			if(cJSON_GetObjectItem(JsonRoot1, "transformer") == NULL)
			{

				JsonOption2 = cJSON_CreateArray();
				cJSON_AddItemToArray(JsonOption2, JsonNode3);
				cJSON_AddItemToObject(JsonRoot1, "transformer", JsonOption2);
			}else
			{
				JsonOption2 = cJSON_GetObjectItem(JsonRoot1, "transformer");
				for(i = 0; i < cJSON_GetArraySize(JsonOption2); i++)
				{
					if(cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2,i), "id")->valueint
							== cJSON_GetObjectItem(JsonNode3, "id")->valueint)
					{
						cJSON_ReplaceItemInArray(JsonOption2, i, JsonNode3);
						flag_exist_repalce = 1;
						break;
					}
				}
				if(flag_exist_repalce == 0)
					cJSON_AddItemToArray(JsonOption2, JsonNode3);
			}

			break;

		default:

			LOG_ERROR(TAG, "Get cmd error\r\n");
			free(buf);
			return -1;
			break;
		}

		if(JsonRoot1 != NULL)
		{
			pthread_mutex_lock(&pthread_mutex_this);
			SaveJsonToFile(JsonRoot1, SHARE_MEM_FIEL);
			pthread_mutex_unlock(&pthread_mutex_this);
		}

//		LOG_DEBUG(TAG,"sharemem json :\r\n%s\r\n", cJSON_Print(JsonRoot1));

	}
	else
	{

		LOG_ERROR(TAG, "Not follow agreement\r\n");
		free(buf);
		return -1;
	}

	free(buf);
	return 0;
}

long  cJSON_Get_ST_ET_time(cJSON *jsonroot, const char *key)
{
	long  time;
	int  i = 0 , j = 0, k = 0;
	char *p, *q;
	if(cJSON_GetObjectItem(jsonroot, key)->valuestring !=  NULL)
	{
	//	LOG_DEBUG(TAG, "%s = %s\r\n", key,  cJSON_GetObjectItem(jsonroot, key)->valuestring);

		if(strstr(cJSON_GetObjectItem(jsonroot, key)->valuestring, ":")  == NULL)
		{
			LOG_DEBUG(TAG, "cJSON_Get_ST_ET_time  use(%s) but  no  flag ':'\r\n", key);
			time = -1;
			return time;
		}else
		{

			i = atoi(cJSON_GetObjectItem(jsonroot, key)->valuestring);

			p = strstr(cJSON_GetObjectItem(jsonroot, key)->valuestring, ":");

			++p;
			j = atoi(p);

			q = strstr(p, ":");

			++q;
			k = atoi(q);

			LOG_DEBUG(TAG, "cJSON_Get_ST_ET_time--> %d:%d:%d\r\n", i, j, k);

			time = i * 3600 + j * 60 + k;

			LOG_DEBUG(TAG, "cJSON_Get_ST_ET_time time = %ld\r\n", time);

		}
	}
	return  time;
}



//3 生成json文件
char  Create_Pkgs(cJSON * JsonRoot1)
{
	cJSON  *JsonOption2, *JsonNode3, *JsonData4, *JsonLast;
	int i = 0,j;
	struct tm  *time_nowtm;
	int buf_int[6];



	struct{
		struct {
			int tm_sec;	   /* seconds */
			int tm_min;	   /* minutes */
			int tm_hour;	   /* hours */
			int tm_mday;	   /* day of the month */
			int tm_mon;	   /* month */
			int tm_year;	   /* year */
			int tm_wday;	   /* day of the week */
			int tm_yday;	   /* day in the year */
			int tm_isdst;	   /* daylight saving time */
		}tm;


	}time_now;
	time_t timer;

	JsonData4 = cJSON_CreateObject();
	time(&(timer));
	time_nowtm= localtime(&timer);
#if 0
	LOG_DEBUG(TAG,"year = %d\r\n", time_nowtm->tm_year + 1900);
	LOG_DEBUG(TAG,"mon = %d\r\n", time_nowtm->tm_mon + 1);
	LOG_DEBUG(TAG,"day = %d\r\n", time_nowtm->tm_mday);
	LOG_DEBUG(TAG,"hour = %d\r\n", time_nowtm->tm_hour);
	LOG_DEBUG(TAG,"min = %d\r\n", time_nowtm->tm_min);
	LOG_DEBUG(TAG,"sec = %d\r\n", time_nowtm->tm_sec);
#endif

	cJSON_AddNumberToObject(JsonData4, "year",time_nowtm->tm_year + 1900 );
	cJSON_AddNumberToObject(JsonData4, "mon",time_nowtm->tm_mon + 1 );
	cJSON_AddNumberToObject(JsonData4, "day",time_nowtm->tm_mday );
	cJSON_AddNumberToObject(JsonData4, "hour",time_nowtm->tm_hour );
	cJSON_AddNumberToObject(JsonData4, "minute",time_nowtm->tm_min );
	cJSON_AddNumberToObject(JsonData4, "second",time_nowtm->tm_sec );


	JsonNode3 = cJSON_CreateObject();

	cJSON_AddStringToObject(JsonNode3, "zigbeeport","/dev/ttyS1");
	cJSON_AddStringToObject(JsonNode3, "webport","/dev/ttyS2");

	cJSON_AddNumberToObject(JsonNode3, "ver",104);
	// h * 100 + l * 1
	cJSON_AddNumberToObject(JsonNode3, "cver",3);
	cJSON_AddNumberToObject(JsonNode3, "port",8080);

	bzero(buf_int,sizeof(buf_int));
	buf_int[0] = 192;
	buf_int[1] = 168;
	buf_int[2] = 1;
	buf_int[3] = 110;

	cJSON_AddItemToObject(JsonNode3, "ip",cJSON_CreateIntArray(buf_int,4));
	cJSON_AddItemToObject(JsonNode3,"datetime", JsonData4);
	cJSON_AddItemToObject(JsonRoot1, "config",JsonNode3);


	//	cJSON_Delete(JsonNode3);
	//	cJSON_Delete(JsonData4);


	JsonOption2 = cJSON_CreateArray();
	/*
	   JsonNode3 = cJSON_CreateObject();


	   for(i = 1 ;i < 6; i ++)	
	   {
	   buf_int[i - 1] = i;
	   }

	   cJSON_AddItemToObject(JsonNode3, "a7",cJSON_CreateIntArray(buf_int,5));
	   cJSON_AddItemToObject(JsonNode3, "a6",cJSON_CreateIntArray(buf_int,2));

	   cJSON_AddNumberToObject(JsonNode3, "a2",1);
	   cJSON_AddNumberToObject(JsonNode3, "a4",1);
	   cJSON_AddNumberToObject(JsonNode3, "a5",0);
	   cJSON_AddNumberToObject(JsonNode3, "dc",0);
	   cJSON_AddNumberToObject(JsonNode3, "cs",0);
	   cJSON_AddNumberToObject(JsonNode3, "temps",1);
	   cJSON_AddNumberToObject(JsonNode3, "tempx",1);
	   cJSON_AddNumberToObject(JsonNode3, "humiditys",1);
	   cJSON_AddNumberToObject(JsonNode3, "humidityx",1);
	   cJSON_AddNumberToObject(JsonNode3, "co2s",1);
	   cJSON_AddNumberToObject(JsonNode3, "co2x",1);
	   cJSON_AddNumberToObject(JsonNode3, "illumins",1);
	   cJSON_AddNumberToObject(JsonNode3, "illuminx",1);

	   cJSON_AddNumberToObject(JsonNode3, "oilHumiditys",1);
	   cJSON_AddNumberToObject(JsonNode3, "oilHumidityx",1);

	   JsonOption2 = cJSON_CreateArray();
	   cJSON_AddItemToArray(JsonOption2, JsonNode3);
	   */
	cJSON_AddItemToObject(JsonRoot1,"collector", JsonOption2);
	/*
	   JsonNode3 = cJSON_CreateObject();


	   for(i = 1 ;i < 6; i ++)	
	   {
	   buf_int[i - 1] = i;
	   }



	   cJSON_AddItemToObject(JsonNode3, "a7",cJSON_CreateIntArray(buf_int,5));
	   cJSON_AddItemToObject(JsonNode3, "a6",cJSON_CreateIntArray(buf_int,2));

	   cJSON_AddNumberToObject(JsonNode3, "a2",2);
	   cJSON_AddNumberToObject(JsonNode3, "a4",1);
	   cJSON_AddNumberToObject(JsonNode3, "a5",0);
	   cJSON_AddNumberToObject(JsonNode3, "dc",0);
	   cJSON_AddNumberToObject(JsonNode3, "cs",0);
	   cJSON_AddNumberToObject(JsonNode3, "temps",2);
	   cJSON_AddNumberToObject(JsonNode3, "tempx",2);
	   cJSON_AddNumberToObject(JsonNode3, "humiditys",2);
	   cJSON_AddNumberToObject(JsonNode3, "humidityx",2);
	   cJSON_AddNumberToObject(JsonNode3, "co2s",2);
	   cJSON_AddNumberToObject(JsonNode3, "co2x",2);

	   cJSON_AddNumberToObject(JsonNode3, "illumins",2);
	   cJSON_AddNumberToObject(JsonNode3, "illuminx",2);
	   cJSON_AddNumberToObject(JsonNode3, "oilHumiditys",2);
	   cJSON_AddNumberToObject(JsonNode3, "oilHumidityx",2);


	   cJSON_AddItemToArray(JsonOption2, JsonNode3);
	//	LOG_DEBUG(TAG,"%s\r\n", cJSON_Print(JsonOption2));
	*/

	cJSON_AddItemToObject(JsonRoot1, "controller", (JsonOption2 = cJSON_CreateArray()));
	/*
	   cJSON_AddItemToArray(JsonOption2,  (JsonNode3 = cJSON_CreateObject()));
	   cJSON_AddNumberToObject(JsonNode3, "conid", 1);

	   cJSON_AddItemToObject(JsonNode3, "dev", (JsonData4 = cJSON_CreateArray()) );
	   cJSON_AddItemToArray(JsonData4, (JsonLast = cJSON_CreateObject()));


	   cJSON_AddNumberToObject(JsonLast, "devid", 1);
	   cJSON_AddStringToObject(JsonLast, "a6", "1");
	   cJSON_AddNumberToObject(JsonLast, "type", 1);

	   cJSON_AddItemToObject(JsonLast, "hgq", cJSON_CreateIntArray(buf_int,2));

	   buf_int[0] = 1;
	   buf_int[1] = 0x11;

	   buf_int[2] = 2;
	   buf_int[3] = 0x11;

	   buf_int[4] = 3;
	   buf_int[5] = 0x11;

	   cJSON_AddItemToObject(JsonLast, "relay", cJSON_CreateIntArray(buf_int,6));

	   cJSON_AddNumberToObject(JsonLast, "glsen", 1);
	   cJSON_AddNumberToObject(JsonLast, "glcollector", 3);
	   cJSON_AddNumberToObject(JsonLast, "acon", 2);
	   cJSON_AddStringToObject(JsonLast, "re1", "10");
	   cJSON_AddStringToObject(JsonLast, "re2", "40");

	   cJSON_AddStringToObject(JsonLast, "st1", "10:00");
	   cJSON_AddStringToObject(JsonLast, "et1", "11:00");
	   cJSON_AddStringToObject(JsonLast, "st2", "12:00");
	   cJSON_AddStringToObject(JsonLast, "et2", "14:00");
	   cJSON_AddStringToObject(JsonLast, "st3", "16:00");
	   cJSON_AddStringToObject(JsonLast, "et3", "18:00");
	   */
	//	LOG_DEBUG(TAG,"jsonroot-----------------------------\r\n");
	//	LOG_DEBUG(TAG,"%s\r\n", cJSON_Print(JsonRoot1));

	return 1;
}
void SaveJsonToFile(cJSON *json, const char* file)
{
	char *out;
	FILE* fp = fopen(file, "w+");

	out=cJSON_Print(json);
	LOG_DEBUG(TAG,"SaveJsonToFile [%s] = \r\n%s\r\n",file,  out);
	fprintf(fp, out);   
	fclose(fp);

	free(out);

	out = NULL;
}



int cJSON_Change_IntNumber(cJSON *json, const char *key, int value)
{

	if(json != NULL)
	{
		cJSON_GetObjectItem(json,key)->valueint = value;
		cJSON_GetObjectItem(json,key)->valuedouble = value;
		return 0;
	}

	return  -1;
}


char Read_Caijiqi_From_Json(unsigned char collectorid, char (buf[]), int *len)
{

	cJSON *JsonRoot1, *JsonOption2, *JsonNode3;
	int i = 0, j = 0;
	if((JsonRoot1 = GetJsonObject(SHARE_MEM_FIEL)) == NULL)
	{
		LOG_ERROR(TAG, "Read_Caijiqi_From_Json error jsonroot is null");
		return -1;
	}else{

		JsonOption2 = cJSON_GetObjectItem(JsonRoot1, "collector");
		if(JsonOption2 != NULL)
		{
			if(collectorid == 0)
			{

				for(i = 0; i < cJSON_GetArraySize(JsonOption2); i++)
				{	
					// todo  if not this id
					(buf[j++]) = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "id")->valueint ;

					JsonNode3 = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "data");

					if(JsonNode3 != NULL)
					{
						*len = cJSON_GetArraySize(JsonNode3); 
						(buf[j++]) = *len; 

						for(i = 0; i < *len; i++)
							(buf[j++]) = cJSON_GetArrayItem(JsonNode3, i)->valueint;

						(*len) = j;
					}else
						return -1;

				}
			}
			else
			{
				for(i = 0; i < cJSON_GetArraySize(JsonOption2); i++)
				{
					LOG_ERROR(TAG, "id = %d\r\n",
							cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "id")->valueint );
					// todo  if not this id
					if(cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "id")->valueint == collectorid)
					{
						(buf[0]) = collectorid;

						JsonNode3 = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "data");

						if(JsonNode3 != NULL)
						{
							*len = cJSON_GetArraySize(JsonNode3); 
							(buf[1]) = *len; 

							for(i = 0; i < *len; i++)
								(buf[i + 2]) = cJSON_GetArrayItem(JsonNode3, i)->valueint;

							(*len) +=  2;
						}else
							return -1;
					}

				}
			}

		}

		return 1;
	}
}


char Read_Huganqi_From_Json(unsigned char collectorid, char (buf[]), int *len)
{

	cJSON *JsonRoot1, *JsonOption2, *JsonNode3;
	int i = 0, j = 0;
	if((JsonRoot1 = GetJsonObject(SHARE_MEM_FIEL)) == NULL)
	{
		LOG_ERROR(TAG, "Read_Caijiqi_From_Json error jsonroot is null");
		return -1;
	}else{

		JsonOption2 = cJSON_GetObjectItem(JsonRoot1, "transformer");
		if(JsonOption2 != NULL)
		{
			for(i = 0; i < cJSON_GetArraySize(JsonOption2); i++)
			{
				if(cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "id") != NULL)
				{

					LOG_ERROR(TAG, "id = %d\r\n",
							cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "id")->valueint );
					// todo  if not this id

					if((cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "id")->valueint == collectorid) )
					{
						(buf[j++]) = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "id")->valueint;
						JsonNode3 = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "data");
						if(JsonNode3 !=  NULL)
						{
							*len = cJSON_GetArraySize(JsonNode3); 
							(buf[j++]) = *len; 

							for(i = 0; i < *len; i++)
								(buf[j++]) = cJSON_GetArrayItem(JsonNode3, i)->valueint;
						}else
							return -1;
						break;
					}else if(collectorid == 0 )
					{

						(buf[j++]) = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "id")->valueint;

						JsonNode3 = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "data");
						if(JsonNode3 !=  NULL)
						{
							*len = cJSON_GetArraySize(JsonNode3); 
							(buf[j++]) = *len; 

							for(i = 0; i < *len; i++)
								(buf[j++]) = cJSON_GetArrayItem(JsonNode3, i)->valueint;
						}else
							return -1;
					}
				}
			}
			*len = j;
		}

		return 1;
	}
}


char Save_Control_To_Json(unsigned char collectorid, char *buf, int len)
{

	cJSON *JsonRoot1, *JsonOption2, *JsonNode3;
	int i = 0, j = 0;
	char flag_exist_id = 0;
	int  buf_int[128] = {0};
	for(i = 0; i < len; i ++)
	{
		buf_int[i] = (buf[i]);
		printf("%02x  %02x ", buf_int[i], buf[i]);
	}
	printf("\r\n");

	printf("Save_Control_To_Json  id = %d buf[0] = %02x buf[1] = %02x, len = %d\r\n", collectorid, buf[0], buf[1], len);
	if((JsonRoot1 = GetJsonObject(SHARE_MEM_FIEL)) == NULL)
	{
		LOG_ERROR(TAG, "SHARE_MEM_FIEL  is null");
		return -1;
	}else{

		LOG_DEBUG(TAG,"Save_Control_To_Json\r\n");

		JsonOption2 = cJSON_GetObjectItem(JsonRoot1, "controllercmd");
		if(JsonOption2 != NULL)
		{

			for(i = 0; i < cJSON_GetArraySize(JsonOption2); i++)
			{
				LOG_ERROR(TAG, "id = %d\r\n",
						cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "id")->valueint );
				// todo  if not this id
				if(cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "id")->valueint == collectorid)
				{			
					if(cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "cmd") != NULL)
					{
						//	cJSON_ReplaceItemInObject(cJSON_GetArrayItem(JsonOption2, i), "cmd", cJSON_CreateIntArray(buf_int,len));
						for(j = 0; j < len; j ++)
							cJSON_AddItemToArray(cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "cmd"), cJSON_CreateNumber(buf_int[j]));
					}
					else	
						cJSON_AddItemToObject(cJSON_GetArrayItem(JsonOption2, i), "cmd", cJSON_CreateIntArray(buf_int,len));
					flag_exist_id = 1;
					break;
				}
			}
			if(flag_exist_id == 0)
			{
				JsonNode3 = cJSON_CreateObject();
				cJSON_AddNumberToObject(JsonNode3, "id", collectorid);
				cJSON_AddItemToObject(JsonNode3, "cmd", cJSON_CreateIntArray(buf_int, len));
				cJSON_AddItemToArray(JsonOption2,  JsonNode3);
			}
		}else
		{
			cJSON_AddItemToObject(JsonRoot1, "controllercmd", (JsonOption2 = cJSON_CreateArray()));
			JsonNode3 = cJSON_CreateObject();
			cJSON_AddNumberToObject(JsonNode3, "id", collectorid);

			cJSON_AddItemToObject(JsonNode3, "cmd", cJSON_CreateIntArray(buf_int, len));
			cJSON_AddItemToArray(JsonOption2, JsonNode3);
		}


		pthread_mutex_lock(&pthread_mutex_this);
		SaveJsonToFile(JsonRoot1, SHARE_MEM_FIEL);
		pthread_mutex_unlock(&pthread_mutex_this);
		return 1;
	}
}


char Get_Transformer_Json_Data(unsigned char *buf, int * len)
{

	cJSON *JsonRoot1, *JsonOption2;
	int i = 0;
	if((JsonRoot1 = GetJsonObject(SHARE_MEM_FIEL)) == NULL)
	{
		LOG_ERROR(TAG, "Get_Transformer_Json_Data error jsonroot is null\r\n");
		return -1;
	}else{
		LOG_DEBUG(TAG, "SHARE_MEM_FIEL = \r\n%s\r\n", cJSON_Print(JsonRoot1));
		JsonOption2 = cJSON_GetObjectItem(JsonRoot1, "transformer");
		if(JsonOption2 == NULL)
		{
			LOG_ERROR(TAG, "Get_Transformer_Json_Data error transformer is null\r\n");
			return -1;
		}else{

			*len  = cJSON_GetArraySize(JsonOption2);
			for(i = 0; i < *len;  i++)
			{
				if(cJSON_GetObjectItem( cJSON_GetArrayItem(JsonOption2, i), "id")!=  NULL)
					buf[i] = cJSON_GetObjectItem( cJSON_GetArrayItem(JsonOption2, i), "id")->valueint;
				else
					return -1;
			}
		}
	}
	return 1;
}

char Get_Collecter_Json_Data(unsigned char *buf, int *len , int * collectorid)
{
	cJSON *JsonRoot1, *JsonOption2, *JsonNode3;
	static cJSON *json_temp = NULL;
	static char next_collector_id = 0;
	int i = 0;
	if((JsonRoot1 = GetJsonObject(SHARE_MEM_FIEL)) == NULL)
	{
		LOG_ERROR(TAG, "Get_Controller_Json_Data error jsonroot is null");

	}else{
		JsonOption2 = cJSON_GetObjectItem(JsonRoot1, "collector");
		if(JsonOption2 == NULL)
		{
			LOG_ERROR(TAG, "Get_Controller_Json_Data error collector is null");
		}else{

			if(next_collector_id == 0)
			{
				json_temp = JsonOption2->child;
				LOG_DEBUG(TAG, "next_collector_id == 0; json_temp = \r\n%s\r\n", cJSON_Print(json_temp));
				next_collector_id = 1;
			}


			LOG_DEBUG(TAG, " ------>  json_temp = \r\n%s\r\n", cJSON_Print(json_temp));
			if(json_temp != NULL)
			{

				if(cJSON_GetObjectItem(json_temp, "id")!=  NULL)
				{
					*collectorid = cJSON_GetObjectItem( json_temp, "id")->valueint;
					if(	(JsonNode3 = cJSON_GetObjectItem( json_temp, "type")) != NULL) 
					{

						*len = cJSON_GetArraySize(JsonNode3) ;

						LOG_DEBUG(TAG, "collector len = %d\r\n", *len);
						LOG_DEBUG(TAG, "collector id = %d\r\n", *collectorid);
						for(i = 0 ; i < (*len); i ++)
						{
							buf[i] = cJSON_GetArrayItem(JsonNode3, i)->valueint;
							LOG_DEBUG(TAG, "collector type = %d\r\n", buf[i]);
						}

						json_temp = json_temp->next;
						if(json_temp != NULL)
							LOG_DEBUG(TAG, " json_temp = \r\n%s\r\n", cJSON_Print(json_temp));
						return 1;
					}
				}

				json_temp = json_temp->next;
				if(json_temp != NULL)
					LOG_DEBUG(TAG, "break, json_temp = \r\n%s\r\n", cJSON_Print(json_temp));
				else
				{
					LOG_DEBUG(TAG, "next_collector_id = 0 return -1 :1\r\n" );
					next_collector_id = 0;
					return -1;
				}
			}
			else
			{
				LOG_DEBUG(TAG, "next_collector_id = 0 return -1 :2\r\n" );
				next_collector_id = 0;
				return -1;
			}
		}
	}
	return -1;
}

char Get_Controller_Json_Data(unsigned char *buf, int *len , int * controllerid)
{
	cJSON *JsonRoot1, *JsonOption2, *JsonNode3;
	static cJSON *json_temp = NULL;
	static char next_controller_id = 0;
	int i = 0;
	if((JsonRoot1 = GetJsonObject(SHARE_MEM_FIEL)) == NULL)
	{
		LOG_ERROR(TAG, "Get_Controller_Json_Data jsonroot is null");
	}else{
		JsonOption2 = cJSON_GetObjectItem(JsonRoot1, "controllercmd");
		if(JsonOption2 == NULL)
		{
			//LOG_ERROR(TAG, "Get_Transformer_Json_Data error control is null");
		}else{

			if(next_controller_id == 0)
			{
				json_temp = JsonOption2->child;
				next_controller_id = 1;
			}
			LOG_DEBUG(TAG, " Get_Controller_Json_Data------>  json_temp = \r\n%s\r\n", cJSON_Print(json_temp));
			if(json_temp != NULL)
			{
				if(cJSON_GetObjectItem(json_temp, "id")!=  NULL)
				{
					*controllerid = cJSON_GetObjectItem( json_temp, "id")->valueint;
					if(	(JsonNode3 = cJSON_GetObjectItem( json_temp, "cmd")) != NULL) 
					{

						*len = cJSON_GetArraySize(JsonNode3) ;

						LOG_DEBUG(TAG, "controller len = %d\r\n", *len);
						LOG_DEBUG(TAG, "controller id = %d\r\n", *controllerid);
						for(i = 0 ; i < (*len); i ++)
						{
							buf[i] = cJSON_GetArrayItem(JsonNode3, i)->valueint;
							LOG_DEBUG(TAG, "controller relay = %d\r\n", buf[i]);
						}

						json_temp = json_temp->next;
						if(json_temp != NULL)
							LOG_DEBUG(TAG, " json_temp = \r\n%s\r\n", cJSON_Print(json_temp));
						return 1;
					}
				}

				json_temp = json_temp->next;
				if(json_temp != NULL)
					LOG_DEBUG(TAG, "break, json_temp = \r\n%s\r\n", cJSON_Print(json_temp));
				else
				{
					LOG_DEBUG(TAG, "next_controller_id = 0 return -1 :1\r\n" );
					next_controller_id = 0;
					cJSON_DeleteItemFromObject(JsonRoot1, "controllercmd");

					pthread_mutex_lock(&pthread_mutex_this);
					SaveJsonToFile(JsonRoot1, SHARE_MEM_FIEL);
					pthread_mutex_unlock(&pthread_mutex_this);

					return -1;
				}
			}
			else
			{
				LOG_DEBUG(TAG, "next_controller_id = 0 return -1 :2\r\n" );
				next_controller_id = 0;
				cJSON_DeleteItemFromObject(JsonRoot1, "controllercmd");
				pthread_mutex_lock(&pthread_mutex_this);
				SaveJsonToFile(JsonRoot1, SHARE_MEM_FIEL);
				pthread_mutex_unlock(&pthread_mutex_this);
				return -1;
			}
		}
	}
	return -1;
}

char cjson_from_config_to_sharemem(void)
{

	cJSON *JsonRoot1, *JsonOption2, *JsonNode3, *jsontemp;
	cJSON *share_jsonroot, *share_option2, *share_node3;
	cJSON *share_json_trans2, *share_json_trans3;
	unsigned char i = 0, j = 0, k = 0;
	int buf_int[32], temp;

	if((JsonRoot1 = GetJsonObject(JSON_CONFIG_FILE)) == NULL)
	{
		LOG_ERROR(TAG, "cjson_from_config_to_sharemem error jsonroot is null\r\n");
		return -1;
	}else{

		LOG_ERROR(TAG, "cjson_from_config_to_sharemem ok  jsonroot try to full share_jsonroot\r\n");

		share_jsonroot = cJSON_CreateObject();
		share_option2 = cJSON_CreateArray();

		if((JsonOption2 = cJSON_GetObjectItem(JsonRoot1, "collector"))!= NULL)
		{
			for(i = 0; i < cJSON_GetArraySize(JsonOption2); i++)
			{

				share_node3 = cJSON_CreateObject();

				cJSON_AddNumberToObject(share_node3, "id", 
						cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "a2")->valueint
						);
				jsontemp = 	cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "a7");

				memset(buf_int, 0, sizeof(buf_int));
				for(j = 0; j < cJSON_GetArraySize(jsontemp); j ++)
				{
					buf_int[j] = cJSON_GetArrayItem(jsontemp, j)->valueint;
					//printf("%d ", buf_int[j]);
				}

				cJSON_AddItemToObject(share_node3, "type",
						cJSON_CreateIntArray(buf_int, cJSON_GetArraySize(jsontemp)));

				cJSON_AddItemToArray(share_option2, share_node3);	
			}
		}

		cJSON_AddItemToObject(share_jsonroot, "collector", share_option2);

		LOG_DEBUG(TAG, "get collector ok share_option2 = \r\n%s\r\n",cJSON_Print(share_option2)); // 
		LOG_DEBUG(TAG, "get collector ok share_jsonroot = \r\n%s\r\n",cJSON_Print(share_jsonroot)); // 

		//		cJSON_Delete(share_option2);
		share_option2 = cJSON_CreateArray();
		share_json_trans2 = cJSON_CreateArray();
		if((JsonOption2 = cJSON_GetObjectItem(JsonRoot1, "controller"))!= NULL)
		{

			LOG_DEBUG(TAG, "1 \r\n"); // 


			for(i = 0; i < cJSON_GetArraySize(JsonOption2); i++)
			{
				if( cJSON_GetArrayItem(JsonOption2, i) != NULL)
				{

					LOG_DEBUG(TAG, "2\r\n"); // 
					share_node3 = cJSON_CreateObject();
					cJSON_AddNumberToObject(share_node3, "id", cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "conid")->valueint);
#if 0
					if((JsonNode3 = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "dev")) != NULL)
					{

						LOG_DEBUG(TAG, "cJSON_GetArraySize(dev) = %d\r\n", cJSON_GetArraySize(JsonNode3)); // 
						for(j = 0; j < cJSON_GetArraySize(JsonNode3); j ++)
						{
							// get hgq  ----------------------------  //


							LOG_DEBUG(TAG, "node3 j= \r\n%s \r\n", cJSON_Print(cJSON_GetArrayItem(JsonNode3, j))); // 
							jsontemp = 	cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j), "hgq");

							LOG_DEBUG(TAG, "hgq jsontemp= \r\n%s \r\n", cJSON_Print(jsontemp)); // 
							LOG_DEBUG(TAG, "5 \r\n"); // 
							memset(buf_int, 0, sizeof(buf_int));
							for(k = 0; k < cJSON_GetArraySize(jsontemp); k ++)
							{
								buf_int[k] = cJSON_GetArrayItem(jsontemp, k)->valueint;

								share_json_trans3 = cJSON_CreateObject();
								//	printf("%d ", buf_int[k]);
								cJSON_AddNumberToObject(share_json_trans3, "id", buf_int[k]);
								cJSON_AddItemToArray(share_json_trans2, share_json_trans3);
							}

							LOG_DEBUG(TAG, "6\r\n"); // 


						}
					}
#endif
					cJSON_AddItemToArray(share_json_trans2, share_node3);
				}
			}
		}

		cJSON_AddItemToObject(share_jsonroot, "transformer", share_json_trans2);


		LOG_DEBUG(TAG, "get transformer ok share_json_trans2 = \r\n%s\r\n",cJSON_Print(share_json_trans2)); // 

		//cJSON_Delete(share_option2);
		share_option2 = cJSON_CreateArray();
		if((JsonOption2 = cJSON_GetObjectItem(JsonRoot1, "controller"))!= NULL)
		{

			LOG_DEBUG(TAG, "1 \r\n"); // 
			for(i = 0; i < cJSON_GetArraySize(JsonOption2); i++)
			{

				if( cJSON_GetArrayItem(JsonOption2, i) != NULL)
				{
					share_node3 = cJSON_CreateObject();
					cJSON_AddNumberToObject(share_node3, "id", cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "conid")->valueint);


#if 0
					LOG_DEBUG(TAG, "2\r\n"); // 
					if((JsonNode3 = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "dev")) != NULL)
					{

						LOG_DEBUG(TAG, "cJSON_GetArraySize(dev) = %d\r\n", cJSON_GetArraySize(JsonNode3)); // 
						for(j = 0; j < cJSON_GetArraySize(JsonNode3); j ++)
						{
							// get relay  ----------------------------  //

							jsontemp = 	cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j), "relay");


							LOG_DEBUG(TAG, "relay = \r\n%s\r\n", cJSON_Print(jsontemp)); // 
							LOG_DEBUG(TAG, "devid = %s\r\n", cJSON_Print(	cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j), "devid"))); // 
							LOG_DEBUG(TAG, "3 \r\n"); // 
							memset(buf_int, 0, sizeof(buf_int));
							for(k = 0; k < cJSON_GetArraySize(jsontemp); k ++)
							{
								buf_int[k] = cJSON_GetArrayItem(jsontemp, k)->valueint;
								printf("relay :%d\r\n", buf_int[k]);
							}
							share_node3 = cJSON_CreateObject();
							cJSON_AddNumberToObject(share_node3, "id", cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j), "devid")->valueint);
							if(cJSON_GetObjectItem(share_node3, "relay") != NULL)
								cJSON_ReplaceItemInObject(share_node3, "relay", cJSON_CreateIntArray(buf_int,cJSON_GetArraySize(jsontemp)));
							else
								cJSON_AddItemToObject(share_node3, "relay",
										cJSON_CreateIntArray(buf_int, cJSON_GetArraySize(jsontemp)));

							LOG_DEBUG(TAG, "4\r\n"); //

							LOG_DEBUG(TAG, "1 share_option2 : \r\n%s \r\n", cJSON_Print(share_option2)); // 
							LOG_DEBUG(TAG, "2 share_node3 : \r\n%s \r\n", cJSON_Print(share_node3)); // 
							cJSON_AddItemToArray(share_option2, share_node3);

							LOG_DEBUG(TAG, "3 share_option2 : \r\n%s \r\n", cJSON_Print(share_option2)); // 
							LOG_DEBUG(TAG, "4.5 \r\n"); // 

						}
					}
#endif

					cJSON_AddItemToArray(share_option2, share_node3);
				}


			}
		}

		cJSON_AddItemToObject(share_jsonroot, "controller", share_option2);

		LOG_DEBUG(TAG, "cjson_from_config_to_sharemem share_jsonroot:\r\n%s\r\n", cJSON_Print(share_jsonroot)); // 

		pthread_mutex_lock(&pthread_mutex_this);
		SaveJsonToFile(share_jsonroot, SHARE_MEM_FIEL);
		pthread_mutex_unlock(&pthread_mutex_this);

		//		while(1);
	}
}
unsigned char get_huganqi_status(int conid, int hgqid) //  0  or 1;  error return 2;
{

	cJSON *JsonRoot1, *JsonOption2, *JsonNode3, *JsonData4;
	int i = 0;

	//LOG_DEBUG(TAG, "get_huganqi_status ...\r\n");
	if((JsonRoot1 = GetJsonObject(SHARE_MEM_FIEL)) == NULL)
	{
		LOG_ERROR(TAG, "get_huganqi_status error jsonroot is null\r\n");
		return 2;
	}else{
		JsonOption2 = cJSON_GetObjectItem(JsonRoot1, "transformer");
		if(JsonOption2 == NULL)
		{
			LOG_ERROR(TAG, "get_huganqi_status error collector is null\r\n");
			return 2;
		}else{
			for(i = 0 ; i < cJSON_GetArraySize(JsonOption2); i ++)
			{
				JsonNode3 = cJSON_GetArrayItem(JsonOption2, i);
				if(JsonNode3 == NULL)
				{
					LOG_ERROR(TAG, "get_huganqi_status error collector (%d) is null\r\n", i);
					continue;
				}else{
					if(((cJSON_GetObjectItem(JsonNode3, "id")) !=  NULL)&&((cJSON_GetObjectItem(JsonNode3, "id")->valueint) == conid))
					{
						JsonData4 = cJSON_GetObjectItem(JsonNode3, "data");
						if(JsonData4 !=NULL)
						{
						//	LOG_DEBUG(TAG, "data[%d] & %02x ", cJSON_GetArrayItem(JsonData4, (hgqid - 1)/8)->valueint, (1 <<( (hgqid - 1)%8)));
						//	if ((cJSON_GetArrayItem(JsonData4, (hgqid - 1)/8)->valueint)&(1 <<(7 - ((hgqid - 1)%8))))
							if ((cJSON_GetArrayItem(JsonData4, (hgqid - 1)/8)->valueint)&(1 <<( (hgqid - 1)%8)))
								return 1;
							else 
								return 0;
						}
					}
				}
			}
		}
	}

	return 2;
}
int Get_Collecter_glsen_data(int id,int glsen)
{
	cJSON *JsonRoot1, *JsonOption2, *JsonNode3, *JsonData4;
	int i = 0, j = 0, k = 0, glsen_data;

	LOG_DEBUG(TAG, "Get_Collecter_glsen_data ...\r\n");
	if((JsonRoot1 = GetJsonObject(SHARE_MEM_FIEL)) == NULL)
	{
		LOG_ERROR(TAG, "Get_Collecter_glsen_data error jsonroot is null\r\n");
		return 0;
	}else{

		JsonOption2 = cJSON_GetObjectItem(JsonRoot1, "collector");
		if(JsonOption2 == NULL)
		{
			LOG_ERROR(TAG, "Get_Collecter_glsen_data error collector is null\r\n");
		}else{

			for(i = 0 ; i < cJSON_GetArraySize(JsonOption2); i ++)
			{

				if(cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "id")->valueint == id)
				{

					JsonNode3 = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "type");
					if(JsonNode3 == NULL)
					{
						LOG_ERROR(TAG, "Get_Collecter_glsen_data error type is null\r\n");
					}else{

						for(j = 0; j < cJSON_GetArraySize(JsonNode3); j ++)
						{

							if(cJSON_GetArrayItem(JsonNode3, j)->valueint == glsen)
							{

								JsonData4 = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "data");
								if(JsonData4 != NULL)
								{

									LOG_DEBUG(TAG, "glsen data -> %d %d\r\n",
											cJSON_GetArrayItem(JsonData4, j * 3 + 1)->valueint,
											cJSON_GetArrayItem(JsonData4, j * 3 + 2)->valueint);

									glsen_data = ((cJSON_GetArrayItem(JsonData4, j * 3 + 1)->valueint) << 8) + cJSON_GetArrayItem(JsonData4, j * 3 + 2)->valueint;

									LOG_DEBUG(TAG, "Get_Collecter_glsen_data glsen_data = %d\r\n", glsen_data);
									return glsen_data; 
								}

							}

						}

					}
				}
			}
		}
	}
	return 0;
}


char send_data_to_web_on_time_use_json_collector(char *buf, int *len)
{

	cJSON *JsonRoot1, *JsonOption2, *JsonNode3;
	int i = 0, j = 0, k = 0;
	if((JsonRoot1 = GetJsonObject(SHARE_MEM_FIEL)) == NULL)
	{
		LOG_ERROR(TAG, "send_data_to_web_on_time_use_json error jsonroot is null");
		return -1;
	}else{

		JsonOption2 = cJSON_GetObjectItem(JsonRoot1, "collector");

		LOG_DEBUG(TAG, "send_data_to_web_on_time_use_json controller =\r\n%s\r\n", 
				cJSON_Print(JsonOption2));

		if(JsonOption2 != NULL)
		{
			if(cJSON_GetArraySize(JsonOption2) == 0)
				return -1;
			for(i = 0; i < cJSON_GetArraySize(JsonOption2); i++)
			{
				if(cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "id") != NULL)
				{
					LOG_DEBUG(TAG, "id = %d\r\n",
							cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "id")->valueint );


					JsonNode3 = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "data");
					LOG_DEBUG(TAG, "send_data_to_web_on_time_use_json controller data =\r\n%s\r\n", 
							cJSON_Print(JsonNode3));
					if(JsonNode3 !=  NULL)
					{
						(buf[j++]) = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "id")->valueint;
						(*len) = cJSON_GetArraySize(JsonNode3); 
						(buf[j++]) = (*len); 

						for(k = 0; k < (*len); k++)
							(buf[j++]) = cJSON_GetArrayItem(JsonNode3, k)->valueint;
					}
				}
			}
			*len  = j;
		}else
			return -1;
	}
	LOG_DEBUG(TAG, "*len = %d\r\n", *len);
	return 1;
}


char send_data_to_web_on_time_use_json_transformer(char *buf, int *len)
{

	cJSON *JsonRoot1, *JsonOption2, *JsonNode3;
	int i = 0, j = 0, k = 0;
	if((JsonRoot1 = GetJsonObject(SHARE_MEM_FIEL)) == NULL)
	{
		LOG_ERROR(TAG, "send_data_to_web_on_time_use_json_transformer error jsonroot is null");
		return -1;
	}else{

		JsonOption2 = cJSON_GetObjectItem(JsonRoot1, "transformer");

		LOG_DEBUG(TAG, "send_data_to_web_on_time_use_json transformer =\r\n%s\r\n", 
				cJSON_Print(JsonOption2));

		if(JsonOption2 != NULL)
		{
			if(cJSON_GetArraySize(JsonOption2) == 0)
				return -1;
			for(i = 0; i < cJSON_GetArraySize(JsonOption2); i++)
			{
				if(cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "id") != NULL)
				{

					LOG_DEBUG(TAG, "id = %d\r\n",
							cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "id")->valueint );


					JsonNode3 = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "data");
					if(JsonNode3 !=  NULL)
					{
						(buf[j++]) = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "id")->valueint;
						(*len) = cJSON_GetArraySize(JsonNode3); 
						(buf[j++]) = (*len); 

						for(k = 0; k < (*len); k++)
							(buf[j++]) = cJSON_GetArrayItem(JsonNode3, k)->valueint;
					}
				}

			}
			*len  = j;
		}else
			return -1;
	}

	LOG_DEBUG(TAG, "*len = %d\r\n", *len);
	return 1;
}
#if 0

if((JsonOption2 = cJSON_GetObjectItem(JsonRoot1, "controller"))!= NULL)
{

	LOG_DEBUG(TAG, "1 \r\n"); // 
	for(i = 0; i < cJSON_GetArraySize(JsonOption2); i++)
	{
		if( cJSON_GetArrayItem(JsonOption2, i) != NULL)
		{

			LOG_DEBUG(TAG, "2\r\n"); // 
			share_node3 = cJSON_CreateObject();
			cJSON_AddNumberToObject(share_node3, "id", cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "conid")->valueint);


			if((JsonNode3 = cJSON_GetObjectItem(cJSON_GetArrayItem(JsonOption2, i), "dev")) != NULL)
			{

				LOG_DEBUG(TAG, "cJSON_GetArraySize(dev) = %d\r\n", cJSON_GetArraySize(JsonNode3)); // 
				for(j = 0; j < cJSON_GetArraySize(JsonNode3); j ++)
				{
					// get hgq  ----------------------------  //


					LOG_DEBUG(TAG, "node3 j= \r\n%s \r\n", cJSON_Print(cJSON_GetArrayItem(JsonNode3, j))); // 
					jsontemp = 	cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j), "hgq");

					LOG_DEBUG(TAG, "hgq jsontemp= \r\n%s \r\n", cJSON_Print(jsontemp)); // 
					LOG_DEBUG(TAG, "5 \r\n"); // 
					memset(buf_int, 0, sizeof(buf_int));
					for(k = 0; k < cJSON_GetArraySize(jsontemp); k ++)
					{
						buf_int[k] = cJSON_GetArrayItem(jsontemp, k)->valueint;

						share_json_trans3 = cJSON_CreateObject();
						//	printf("%d ", buf_int[k]);
						cJSON_AddNumberToObject(share_json_trans3, "id", buf_int[k]);
						cJSON_AddItemToArray(share_json_trans2, share_json_trans3);
					}

					LOG_DEBUG(TAG, "6\r\n"); // 
					// get relay  ----------------------------  //

					jsontemp = 	cJSON_GetObjectItem(cJSON_GetArrayItem(JsonNode3, j), "relay");

					LOG_DEBUG(TAG, "3 \r\n"); // 
					memset(buf_int, 0, sizeof(buf_int));
					for(k = 0; k < cJSON_GetArraySize(jsontemp); k ++)
					{
						buf_int[k] = cJSON_GetArrayItem(jsontemp, k)->valueint;
						printf("relay :%d\r\n", buf_int[k]);
					}

					if(cJSON_GetObjectItem(share_node3, "relay") != NULL)
						cJSON_ReplaceItemInObject(share_node3, "relay", cJSON_CreateIntArray(buf_int,cJSON_GetArraySize(jsontemp)));
					else
						cJSON_AddItemToObject(share_node3, "relay",
								cJSON_CreateIntArray(buf_int, cJSON_GetArraySize(jsontemp)));

					LOG_DEBUG(TAG, "4\r\n"); //

					LOG_DEBUG(TAG, "1 share_option2 : \r\n%s \r\n", cJSON_Print(share_option2)); // 
					LOG_DEBUG(TAG, "2 share_node3 : \r\n%s \r\n", cJSON_Print(share_node3)); // 
					cJSON_AddItemToArray(share_option2, share_node3);

					LOG_DEBUG(TAG, "3 share_option2 : \r\n%s \r\n", cJSON_Print(share_option2)); // 
					LOG_DEBUG(TAG, "4.5 \r\n"); // 

				}
			}

		}
	}
}



#endif

