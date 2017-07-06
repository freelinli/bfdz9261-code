#ifndef __TASK_TTYS2_DTU_WEB_H__
#define __TASK_TTYS2_DTU_WEB_H__



union{
	unsigned char Buf_Send[512];
	struct{
		unsigned char header1 ;
		unsigned char header2 ;
		unsigned char id ;
		unsigned char cmd ;
		unsigned char len_h ;
		unsigned char len_l ;
		unsigned char Buf_Send_WebData[506];
	}buf_all_data;

}Send_Web_All_Data;


int Recv_Function(int fd, unsigned char buf[], int len);

int Task_TTYS2_Dtu_Web(void);



int function_Cmd_Set_Dpcontrol_V3(char buf[],int len, int *controllerid, int *devid);

#endif
