
#include "server.h"
#include "mjpeg.h"

void *server_thread(void *arg)
{
	int fd = server_init();
	int sock_fd = accept_client(fd);
	extern struct CAM jpeg;

	unsigned int piclen = 0;
	char response[24] = {0};
	unsigned char *buf;

	char msg[3] = {0};
	int ret = 0;
	int ret1 = 0;
	while(1)
	{
		ret = read(sock_fd,msg,3);
		printf("msg:%s\n",msg);
		
		piclen = jpeg.len;
		printf("piclen = %d\n",piclen);
		snprintf(response,sizeof(response),"%dlen",piclen);

		buf = (unsigned char *)malloc(piclen);
		memset(buf,0,piclen);
		memcpy(buf,jpeg.buf,piclen);

		ret = write(sock_fd,response,24);
		printf("ret = %d\n",ret);
		ret1 = write(sock_fd,buf,piclen);
		if(ret1 < 0)
		{
			break;
		}
		printf("ret1 = %d\n",ret1);
	}

	close(fd);
	close(sock_fd);

	return 0;
}

