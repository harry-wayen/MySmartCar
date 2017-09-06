
#ifndef _SERVER_H_
#define _SERVER_H_

#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

int server_init();
int accept_client(int fd);
void *server_thread(void *arg);


#endif

