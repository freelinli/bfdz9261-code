
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/wait.h>

#include "log.h"
/**
 * update
 */
const char res_str_update_dir[] =
	"/job";

const char res_str_update_fw_zip_tmp_path[] =
	"/mnt/nand/fw_update_tmp.tar.gz";

const char res_str_update_fw_zip_sync_path[] =
	"/mnt/nand/ota.tar.gz";

const char res_str_update_fw_zip_normal_path[] =
	"/job/fw_update_normal.tar.gz";

const char res_str_update_fw_unzip_path[] =
	"/job/system_org";

const char res_str_system_dir[] =
	"/job";

const char res_str_system_system_path[] =
	"/job/system";
const char res_str_system_old_path[] =
	"/job/system_old";
static const char TAG[] = "Launcher";
#define CONTENT_URL_MAX 512

static int launcher_load_main(void)
{
	char buf[CONTENT_URL_MAX];
	int ret = 0;
	int status;

	strcpy(buf, res_str_system_system_path);
	strcat(buf, "/main");
	if (access(buf, F_OK) == 0) {	
		status = system(buf);
		if ((status < 0) || (!WIFEXITED(status) || (WEXITSTATUS(status)))) {
			LOG_ERROR(TAG, "system \"%s\" error, status : %d\n", buf, status);
			ret = -1;
			goto exit;
		}
	} else {
		LOG_ERROR(TAG, "no main\n");
	}
exit:
	return ret;
}


static int lanucher_is_update_fw_exist(void)
{
	char buf[CONTENT_URL_MAX];
	int status;
	if (access(res_str_update_fw_zip_sync_path, F_OK) == 0) {
		LOG_DEBUG(TAG, "access(res_str_update_fw_zip_sync_path, F_OK) == 0 ok\r\n");


		return 1;
	} else {
		return 0;
	}
}

static int launcher_backup_update_fw(void)
{
	int ret = 0;
	int status;
	char buf[CONTENT_URL_MAX];

	strcpy(buf, "cp -f ");
	strcat(buf, res_str_update_fw_zip_sync_path);
	strcat(buf, " ");
	strcat(buf, res_str_update_fw_zip_normal_path);
	status = system(buf);
	if ((status < 0) || (!WIFEXITED(status) || (WEXITSTATUS(status)))) {
		LOG_ERROR(TAG, "system \"%s\" error, status : %d\n", buf, status);
		ret = -1;
		goto exit;
	}

	strcpy(buf, "rm -rf ");
	strcat(buf, res_str_update_fw_zip_sync_path);
	status = system(buf);
	if ((status < 0) || (!WIFEXITED(status) || (WEXITSTATUS(status)))) {
		LOG_ERROR(TAG, "system \"%s\" error, status : %d\n", buf, status);
	}
exit:
	return ret;
}


static int launcher_replace_system(void)
{
	char buf[CONTENT_URL_MAX];
	int ret = 0;
	int status;


	if (access(res_str_system_system_path, F_OK) == 0) {	
		strcpy(buf, "mv ");
		strcat(buf, res_str_system_system_path);
		strcat(buf, " ");
		strcat(buf, res_str_system_old_path);

		LOG_DEBUG(TAG, "system: %s\r\n", buf);
		status = system(buf);
		if ((status < 0) || (!WIFEXITED(status) || (WEXITSTATUS(status)))) {
			LOG_ERROR(TAG, "system \"%s\" error, status : %d\n", buf, status);
			ret = -1;
		}
	}

	strcpy(buf, "mv ");
	strcat(buf, res_str_update_fw_unzip_path);
	strcat(buf, " ");
	strcat(buf, res_str_system_system_path);
	LOG_DEBUG(TAG, "system: %s\r\n", buf);
	status = system(buf);
	if ((status < 0) || (!WIFEXITED(status) || (WEXITSTATUS(status)))) {
		LOG_ERROR(TAG, "system \"%s\" error, status : %d\n", buf, status);
		ret = -1;
		goto exit;
	}
	
exit:
	return ret;
}


static int launcher_decompress_update_fw(void)
{
	char buf[CONTENT_URL_MAX];
	int ret = 0;
	int status;

	// rm /mnt/nand1-2/update/system
	if (access(res_str_update_fw_unzip_path, F_OK) == 0) {
		strcpy(buf, "chmod 777 ");
		strcat(buf, res_str_update_fw_unzip_path);
		status = system(buf);
		if ((status < 0) || (!WIFEXITED(status) || (WEXITSTATUS(status)))) {
			LOG_ERROR(TAG, "system \"%s\" error, status : %d\n", buf, status);
			ret = -1;
			goto exit;
		}
	}
		
	// unzip -o /mnt/nand1-2/update/fw-update_normal.zip -d /mnt/nand1-2/update
	strcpy(buf, "tar -zxvf ");
	strcat(buf, res_str_update_fw_zip_normal_path);
	strcat(buf, " -C ");
	strcat(buf, res_str_update_dir);
	status = system(buf);
	if ((status < 0) || (!WIFEXITED(status) || (WEXITSTATUS(status)))) {
		LOG_ERROR(TAG, "system \"%s\" error, status : %d\n", buf, status);
		ret = -1;
		goto exit;
	}
	
exit:
	return ret;
}

int  function_for_launcher(void)
{

	int ret = 0;

	// check new firmware
	if (!lanucher_is_update_fw_exist()) {
		LOG_DEBUG(TAG, "no update firmware ...\n");
		goto exit_lauch_main;
	}

	LOG_DEBUG(TAG, "find update firmware, update system ...\n");
	// backup new firmware
	if (launcher_backup_update_fw() < 0) {
		LOG_ERROR(TAG, "launcher_backup_update_fw error\n");
		ret = -1;
		goto exit_backup_update_fw_failed;
	} 

	// decompress
	if (launcher_decompress_update_fw() < 0) {
		LOG_ERROR(TAG, "launcher_decompress_update_fw error\n");
		ret = -1;
		goto exit_decompress_update_fw_failed;
	}

	// replace
	if (launcher_replace_system() < 0) {
		LOG_ERROR(TAG, "launcher_replace_system error\n");
		ret = -1;
		goto exit_replace_system_failed;
	}
	
	goto exit_update_ok;
exit_replace_system_failed:
	LOG_ERROR(TAG, "update system error: exit_replace_system_failed\n");
	return -1;
exit_decompress_update_fw_failed:

	LOG_ERROR(TAG, "update system error: exit_decompress_update_fw_failed\n");
	return -1;
exit_backup_update_fw_failed:

	LOG_ERROR(TAG, "update system error:exit_backup_update_fw_failed\n");
	sleep(10);
	return  -1;
//	exit(-1);
	
exit_update_ok:
	LOG_DEBUG(TAG, "update system ok\n");
	sleep(2);

	LOG_DEBUG(TAG, "start reboot ........\n");

	system("reboot");
exit_lauch_main:
	//	launcher_load_main();

//		system("umount  -t /dev/mtdblock1");
	return 0;
}


static void *thread_launcher(void *data)
{

	//system("mount -t vfat /dev/mtdblock1 /mnt/nand");
	while(1)
	{
		LOG_DEBUG(TAG, "function_for_launcher start\r\n");
		system("mount -t vfat /dev/mtdblock1 /mnt/nand");
		sleep(5);
		system("ls -l /mnt/nand/");
		function_for_launcher();
		system("umount -f /mnt/nand");
		sleep(5);	

	}
}




int Task_launcher(void)
{
	int ret = 0;
	pthread_t tid;

	ret = pthread_create(&tid, NULL, thread_launcher, NULL);
	if (ret != 0) {
		LOG_ERROR(TAG, "pthread_create error : %d\n", ret);
		ret = -1;
	}
	//	pthread_join(tid,NULL);
	LOG_DEBUG(TAG, "Task_launcher  start\n");

	return ret;
}
