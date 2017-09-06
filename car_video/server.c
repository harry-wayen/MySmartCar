
#include "server.h"

int server_init()
{
	//创建网络通信套接字
	int fd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == fd)
	{
		printf("socket fail!\n");
		return -1;
	}
	printf("socket success!\n");

	//设置绑定信息
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8888);
	server_addr.sin_addr.s_addr = inet_addr("192.168.1.1");
	if(-1 == bind(fd,(struct sockaddr *)&server_addr,sizeof(server_addr)))
	{
		printf("bind fail!\n");
		return -1;
	}
	printf("bing success!\n");

	//监听客户端连接
	if(-1 == listen(fd,8))
	{
		printf("listen fail!\n");
		return -1;
	}
	printf("listening......\n");

	return fd;
}

int accept_client(int fd)
{
	//接收客户端连接
	int sock_fd = accept(fd,NULL,NULL);
	if(-1 == sock_fd)
	{
		printf("accept fail!\n");
		return -1;
	}
	printf("accept success!\n");

	return sock_fd;
}


