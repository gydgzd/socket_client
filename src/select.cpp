/*
 * select.cpp
 *
 *  Created on: Nov 19, 2019
 *      Author: root
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>


int testSelect()
{
    int server_sockfd, client_sockfd;
    unsigned int server_len, client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    int result;
    fd_set readfds, testfds;//readfds用于保存套接字,select函数每次执行后会把集合清零

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);//允许任何ip连接
    server_address.sin_port = htons(9734);
    server_len = sizeof(server_address);

    signal(SIGPIPE,SIG_IGN);//忽略    SIGPIPE 信号,防止客户端关闭后，服务端继续往客户端写数据时导致服务端异常退出


    bind(server_sockfd, (struct sockaddr*)&server_address, server_len);
    listen(server_sockfd, 5);

    FD_ZERO(&readfds);
    FD_SET(server_sockfd, &readfds);//把套接字放在可读集合中

    while(1){
        char ch[1024];
        int fd;
        int nread;

        testfds = readfds;

        printf("server waiting\n");

        result = select(FD_SETSIZE, &testfds,  (fd_set*)0, (fd_set*)0, (struct timeval*)0);//时间指针为0,表示一直阻塞,如果返回值小于1，表示出错
        if(result < 1){
            perror("server5");
            exit(1);
        }

        for(fd = 0; fd < FD_SETSIZE; ++fd){
            if(FD_ISSET(fd, &testfds)){
                if(fd == server_sockfd)
                {//监听套接字可读表示，表示有新连接
                    client_len = sizeof(client_address);
                    client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_address, &client_len);
                    FD_SET(client_sockfd, &readfds);//将客户端套接字放在可读集合中
                    printf("adding client on fd %d\n", client_sockfd);
                }else
                {
                    ioctl(fd, FIONREAD, &nread);

                    if(nread == 0){//读到0字节表示客户端断开连接
                        close(fd);
                        FD_CLR(fd, &readfds);//把套接字从集合中删除
                        printf("removing client on fd %d\n", fd);
                    }
                    else
                    {

                        read(fd, ch, nread);

                        printf("serving cient on fd  %d\n", fd);

                        write(fd, ch, nread);
                    }

                }
            }
        }

    }


}


