
#include "server.h"
#include "mjpeg.h"

pthread_t thread1;
pthread_t thread2;

int main()
{
	int ret = 0;
	if(-1 == pthread_create(&thread1,NULL,server_thread,NULL))
	{
		printf("server_thread fail!\n");
		return -1;
	}

	if(-1 == pthread_create(&thread2,NULL,cam_thread,NULL))
	{
		printf("cam_thread fail!\n");
		return -1;
	}

	ret = pthread_detach(thread2);
	if(ret != 0)
	{
		printf("detach camera thread!\n");
	}
	else
	{
		printf("detach camera thread success!\n");
	}

	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);

	return 0;
}

