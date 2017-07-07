

#ifndef __JSON_FUNCTION__
#define __JSON_FUNCTION__


#include "cJSON.h"



#define  Task_TTYS2_DTU "/dev/ttyS2"
#define JSON_CONFIG_FILE "./configinfo.json"
#define  Task_TTYS1_DEV "/dev/ttyS1"
#define  SHARE_MEM_FIEL "./sharemem.json"


void file_json_decode(cJSON *json);
char  Create_Pkgs(cJSON * JsonRoot1);

void SaveJsonToFile(cJSON *json, const char* file);


cJSON* GetJsonObject(char* fileName);
char  GetValueString(cJSON* json,int id, char* name, char* param);
char  GetValueNumber(cJSON* json,int id, char* name, int *number);


int cJSON_Change_IntNumber(cJSON *json, const char *key, int value);

char Create_Sharemem_Json(char *buf, int len);

char Read_Caijiqi_From_Json(unsigned char collectorid, char (buf[]), int *len);
char Read_Huganqi_From_Json(unsigned char collectorid, char (buf[]), int *len);

char Save_Control_To_Json(unsigned char collectorid, char buf[], int len);

char Get_Transformer_Json_Data(unsigned char *buf, int * len);
char Get_Collecter_Json_Data(unsigned char *buf, int * len , int * collectorid);
char Get_Controller_Json_Data(unsigned char *buf, int *len , int * controllerid);

char cjson_from_config_to_sharemem(void);
#endif

