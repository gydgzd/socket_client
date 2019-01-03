/*
 * fileTransfer_C.cpp
 *  Created on: Jun 16, 2017
 *  Author: gyd
 *  socket传输文件的client端示例程序
 */
#include<stdlib.h>              // for exit
#include<stdio.h>               // for printf
#include<string.h>              // for bzero
#include<sys/types.h>           // for socket
#include<sys/socket.h>          // for socket
#include<netinet/in.h>          // for sockaddr_in
#include<arpa/inet.h>           // inet_pton
#include <unistd.h>             // close()
#include <errno.h>              // strerror
using namespace std;

#define HELLO_WORLD_SERVER_PORT       6666
#define BUFFER_SIZE                   1024
#define FILE_NAME_MAX_SIZE            512

int fileTransfer(int argc, char **argv)
{
	long fileSize = -1;
	long recvSize = 0;
//	struct stat buf;
    if (argc != 2)
    {
        printf("Usage: ./%s ServerIPAddress", argv[0]);
        exit(1);
    }

    // 设置一个socket地址结构client_addr, 代表客户机的internet地址和端口
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET; // internet协议族
    client_addr.sin_addr.s_addr = htons(INADDR_ANY); // INADDR_ANY表示自动获取本机地址
    client_addr.sin_port = htons(0); // auto allocated, 让系统自动分配一个空闲端口

    // 创建用于internet的流协议(TCP)类型socket，用client_socket代表客户端socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
    	printf("Client create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(-1);
    }

    // bind
    if (bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)))
    {
    	printf("Client bind error: %s (errno: %d)\n", strerror(errno), errno);
        exit(-1);
    }

    // 设置一个socket地址结构server_addr,代表服务器的internet地址和端口
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;

    // 服务器的IP地址来自程序的参数
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0 )
    {
    	printf("inet_pton error: %s\n", argv[1]);
        exit(-1);
    }

    server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);
    socklen_t server_addr_length = sizeof(server_addr);

    // connect
    if (connect(client_socket, (struct sockaddr*)&server_addr, server_addr_length) < 0)
    {
        printf("connect to %s error: %s(errno: %d)\n",argv[1], strerror(errno), errno);
        exit(-1);
    }

    char fileName[FILE_NAME_MAX_SIZE + 1] = "";
    printf("Please Input File Name On Server:\n");
    scanf("%s", fileName);

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    strncpy(buffer, fileName, strlen(fileName) > BUFFER_SIZE ? BUFFER_SIZE : strlen(fileName));
    // 发送客户端需要接收的文件名
    send(client_socket, buffer, BUFFER_SIZE, 0);
    // accept file size
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    char size[32] = "";
    strncpy(size, buffer, BUFFER_SIZE > sizeof(size) ? sizeof(size):BUFFER_SIZE);
    fileSize = atoi(size);
    printf("File %s ,size is %ld B\n", fileName, fileSize);
    FILE *fp = fopen(fileName, "w");
    if (fp == NULL)
    {
        printf("ERR: File %s write error: %s (%d)\n", fileName, strerror(errno), errno);
        exit(1);
    }

    // 从服务器端接收数据到buffer中
    memset(buffer, 0, sizeof(buffer));
    int length = 0;
    while( (length = recv(client_socket, buffer, BUFFER_SIZE, 0)) )
    {
    	// print the process
    	recvSize+=length;
    	printf("File transfered %ld B, %3.2f%%\r",recvSize, 100.0*recvSize/fileSize);
    	fflush(stdout);

        if (length < 0)
        {
            printf("Recieve Data From Server %s Failed!", argv[1]);
            break;
        }
        int write_length = fwrite(buffer, sizeof(char), length, fp);
        if (write_length < length)
        {
            printf("File:    %s Write Failed!", fileName);
            break;
        }
        memset(buffer, 0, BUFFER_SIZE);
    }
    printf("\nRecieve File:     %s From Server[%s] Finished!", fileName, argv[1]);
    // 传输完毕，关闭socket
    fclose(fp);
    close(client_socket);
    return 0;
}

