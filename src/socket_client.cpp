//============================================================================
// Name        : socket_client.cpp
// Author      : gyd
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>  // inet_pton

#define MAXLINE 4096
int socket_client(char* server_IP)
{
	int socket_fd, recv_len;
	char recvline[4096] = "", sendline[4096] = "";

	struct sockaddr_in clientaddr;
	if( (socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Create socket error: %s(errno: %d)\n", strerror(errno), errno);
		exit(-1);
	}
	memset(&clientaddr, 0, sizeof(clientaddr));
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_port = htons(8000);
	if( inet_pton(AF_INET, server_IP, &clientaddr.sin_addr) <= 0)
	{
		printf("inet_pton error: %s\n", server_IP);
		exit(-1);
	}
	if( connect(socket_fd, (struct sockaddr*)&clientaddr, sizeof(clientaddr)) < 0)
	{
		printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
		exit(-1);
	}


	while(true)
	{

		printf("send msg to server: \n");
		fgets(sendline, 4096, stdin);
		if( send(socket_fd, sendline, strlen(sendline), 0) < 0)
		{
			printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
			exit(-1);
		}
		if(strcmp(sendline, "exit\n")==0)
		{
			break;
		}
		if((recv_len = recv(socket_fd, recvline, MAXLINE, 0)) == -1)
		{
			perror("recv error");
			exit(-1);
		}
		recvline[recv_len] = '\0';
		printf("Received: %s", recvline);
		memset(sendline, 0, sizeof(sendline));
		memset(recvline, 0, sizeof(recvline));
		printf("socket_fd is %d.\n",socket_fd);
	}
	close(socket_fd);
	return 0;
}

