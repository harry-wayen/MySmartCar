
#include "mjpeg.h"

int mjpeg_init()
{
	//开启设备
	int fd = open("/dev/video0",O_RDWR);
	if(-1 == fd)
	{
		perror("open device fail");
		return -1;
	}
	printf("open device success!\n");
	int ret = 0;

	//检查设备当前属性
	struct v4l2_capability capability;
	ret = ioctl(fd,VIDIOC_QUERYCAP,&capability);
	if(-1 == ret)
	{
		perror("VIDIOC_QUERYCAP");
		return -1;
	}
	printf("VIDIOC_QUERYCAP success!\n");

	if(!(capability.capabilities & V4L2_CAP_VIDEO_CAPTURE))
	{
		perror("no support V4L2_CAP_VIDEO_CAPTURE");
		close(fd);
		return -1;
	}
	printf("V4L2_CAP_VIDEO_CAPTURE success!\n");

	if(!(capability.capabilities & V4L2_CAP_STREAMING))
	{
		perror("no support V4L2_CAP_STREAMING");
		close(fd);
		return -1;
	}
	printf("V4L2_CAP_STREAMING success!\n");

	//设置设备属性
	struct v4l2_format format;
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	format.fmt.pix.width = 320;
	format.fmt.pix.height = 240;
	format.fmt.pix.field = V4L2_FIELD_ANY;
	if(-1 == ioctl(fd,VIDIOC_S_FMT, &format))
	{
		perror("video set format fail!");
		return -1;
	}
	printf("video set format success!\n");

	if(-1 == ioctl(fd,VIDIOC_G_FMT, &format))
	{
		perror("video set format fail!");
		return -1;
	}
	printf("%d * %d\n",format.fmt.pix.width,format.fmt.pix.height);
	
	return fd;
}

int buff_mmap(int fd,unsigned int *size)
{
	int ret;
	//准备申请帧缓存
	reqbufs.count 	= REQBUFS_COUNT;
	reqbufs.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbufs.memory	= V4L2_MEMORY_MMAP;
	ret = ioctl(fd, VIDIOC_REQBUFS, &reqbufs);	
	if (ret == -1) {	
		perror("camera reqbufs");
		close(fd);
		return -1;
	}

	//缓存队列完成映射
	struct v4l2_buffer vbuf;
	int i = 0;
	for (i = 0; i < reqbufs.count; i++)
	{
		//在内核空间申请内存的模型
		memset(&vbuf, 0, sizeof(struct v4l2_buffer));
		vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		vbuf.memory = V4L2_MEMORY_MMAP;
		vbuf.index = i;
		ret = ioctl(fd, VIDIOC_QUERYBUF, &vbuf);
		if (ret == -1) {
			perror("camera querybuf");
			close(fd);
			return -1;
		}

		//缓存队列逐个完成映射
		bufs[i].length = vbuf.length;
		bufs[i].start = mmap(NULL, vbuf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, vbuf.m.offset);
		if (bufs[i].start == MAP_FAILED)
		{
			perror("camera mmap");
			close(fd);
			return -1;
		}
	
		//映射后的缓存区逐个入队等待采集
		vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		vbuf.memory = V4L2_MEMORY_MMAP;
		ret = ioctl(fd, VIDIOC_QBUF, &vbuf);
		if (ret == -1) {
			perror("camera init");
			close(fd);
			return -1;
		}
	}

	*size = bufs[0].length;

	return fd;
}

int camera_start(int fd)
{
	int ret;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	//开启流采集
	ret = ioctl(fd, VIDIOC_STREAMON, &type);
	if (ret == -1) {
		perror("camera->start");
		return -1;
	}
	fprintf(stdout, "camera->start: start capture\n");

	return 0;
}

int camera_dqbuf(int fd, void **buf, unsigned int *size, unsigned int *index)
{
	int ret;
	fd_set fds;
	struct timeval timeout;
	struct v4l2_buffer vbuf;

	while (1) {
		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;
		ret = select(fd + 1, &fds, NULL, NULL, &timeout);
		if (ret == -1) {
			perror("camera->dqbuf");
			if (errno == EINTR)
				continue;
			else
				return -1;
		} else if (ret == 0) {
			fprintf(stderr, "camera->dqbuf: dequeue buffer timeout\n");
			break;
		} else {
			vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			vbuf.memory = V4L2_MEMORY_MMAP;
			ret = ioctl(fd, VIDIOC_DQBUF, &vbuf);
			if (ret == -1) {
				perror("camera->dqbuf");
				break;
			}
			*buf = bufs[vbuf.index].start;
			*size = vbuf.bytesused;
			*index = vbuf.index;

			return 0;
		}
	}
	return -1;
}

int camera_eqbuf(int fd, unsigned int index)
{
	int ret;
	struct v4l2_buffer vbuf;

	vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vbuf.memory = V4L2_MEMORY_MMAP;
	vbuf.index = index;
	//将出队缓存区入队
	ret = ioctl(fd, VIDIOC_QBUF, &vbuf);
	if (ret == -1) {
		perror("camera->eqbuf");
		return -1;
	}

	return 0;
}

int camera_stop(int fd)
{
	int ret;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	//关闭流采集
	ret = ioctl(fd, VIDIOC_STREAMOFF, &type);
	if (ret == -1) {
		perror("camera->stop");
		return -1;
	}
	fprintf(stdout, "camera->stop: stop capture\n");

	return 0;
}

int camera_exit(int fd)
{
	int i;
	int ret;
	struct v4l2_buffer vbuf;

	vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vbuf.memory = V4L2_MEMORY_MMAP;
	//所有缓存区出队
	for (i = 0; i < reqbufs.count; i++) {
		ret = ioctl(fd, VIDIOC_DQBUF, &vbuf);
		if (ret == -1)
			break;
	}
	//所有缓存区断开映射
	for (i = 0; i < reqbufs.count; i++)
		munmap(bufs[i].start, bufs[i].length);

	fprintf(stdout, "camera->exit: camera exit\n");

	return close(fd);
}





















