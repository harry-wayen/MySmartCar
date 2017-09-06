
#include "mjpeg.h"

void *cam_thread(void *arg)
{
	//缓存区数据的长度，即采集到的图片的长度
	unsigned int size;
	//采集图片的起始地址的保存
	char *buf;
	unsigned int index;//缓存队列中缓存区的序号
	int init_fd;
	int com_fd;
	//打开文件的文件描述符
	int file_fp1;
	//拼接文件路径+文件名
	char file_name[32];

	extern struct CAM jpeg;

	unsigned int i = 100;
//外层循环目的是避免循环采集中出现错误，直接重新打开摄像头，避免采集卡顿
	while(1)	
	{
		init_fd = mjpeg_init();
		com_fd = buff_mmap(init_fd,&size);	
		camera_start(com_fd);
		//真正循环采集程序
		while(1)
		{
			if(camera_dqbuf(com_fd, (void **)&buf, &size, &index)<0)
			{
				goto loop;
			}

			jpeg.len = size;
			memset(jpeg.buf,0,MAX_JPG_SIZE);
			memcpy(jpeg.buf,buf,size);

			camera_eqbuf(com_fd,index);		
		}

		loop:	
			camera_stop(com_fd);
			camera_exit(com_fd);		
	}
	return (void *)0;
}

