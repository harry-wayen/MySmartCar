
#ifndef _MJPEG_H_
#define _MJPEG_H_

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
//错误码专用头文件
#include <errno.h>
#include <unistd.h>
#include <linux/videodev2.h>

//申请的保存图片的缓存区大小
#define MAX_JPG_SIZE	(200 * 1024)

//申请的帧缓存的个数
#define	REQBUFS_COUNT	4

//保存图片的缓存区及其大小
struct CAM{
	unsigned char buf[MAX_JPG_SIZE];
	unsigned int len;
};
//保存图片信息的结构体变量
struct CAM jpeg;

//准备申请帧缓存
struct v4l2_requestbuffers reqbufs;


//缓存队列中的缓存区模型
struct cam_buf {
	void *start;  //内核区保存图片信息区域的起始地址
	size_t length; //内核区保存的图片信息长度
};

//缓存队列
struct cam_buf bufs[REQBUFS_COUNT];

//初始化，包括打开设备，查看设备属性，设置设备属性等
int mjpeg_init();
//申请帧缓存，缓存队列映射，映射后的缓存队列入队等待采集
int buff_mmap(int fd,unsigned int *size);
//开启摄像头的流采集
int camera_start(int fd);
//缓存队列出队，使用select，若某一个缓存区采集到了数据，即可出队
int camera_dqbuf(int fd, void **buf, unsigned int *size, unsigned int *index);
//缓存区入队，根据出队的缓存区的序号重新入队
int camera_eqbuf(int fd, unsigned int index);
//关闭流采集
int camera_stop(int fd);
//退出摄像头采集
int camera_exit(int fd);

void *cam_thread(void *arg);

#endif

