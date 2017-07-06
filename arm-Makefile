OBJ=a.out

SOURCE= main.c log.c tty.c crc16.c json_function.c cJSON.c task_ttys1_zigbee_dev.c task_ttys2_dtu_web.c launcher.c sqlite3.c
CROSS_COMPILE=arm-none-linux-gnueabi-
CC := $(CROSS_COMPILE)gcc
ALL:
	$(CC) $(SOURCE) -o $(OBJ)  -I /opt/arm-2010.09.50/arm-none-linux-gnueabi/include/libxml2 -L /opt/arm-2010.09.50/arm-none-linux-gnueabi/libs -lxml2 -lz  -lm  -lpthread -lsqlite3
	cp a.out /mcuzone/nfsroot/basefs/job/main

.PHONY:clean

clean:
	rm -rf $(OBJ) 
