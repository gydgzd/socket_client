/*
 * mysocket_client.h
 *
 *  Created on: Jun 7, 2017
 *      Author: gyd
 */
#pragma once
#ifndef MYSOCKET_CLIENT_H_
#define MYSOCKET_CLIENT_H_

#include <iostream>
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>      // for close , usleep
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>   //inet_pton
#include <fcntl.h>       // fcntl
#include <queue>
#include "Mylog.h"

#define MAXLINE 4096
#define MAXLENGTH 1024*64
#ifndef BYTE
#define BYTE unsigned char
#endif
struct MSGBODY
{
    int type;              // 0:int, 1:string, 2: byte(hex)
    int length;
    BYTE msg[MAXLENGTH];
};
/*
 * use to form a string clientIP:clientPort--> serverIP:serverPort
 */
struct CONNECTION
{
    int  socket_fd ;       //
    int  clientPort ;      //
    int  serverPort ;
    char clientIP[64] ;
    char serverIP[64] ;
};
class MySocket_client
{
public:
    int mn_socket_fd;
    MySocket_client();
    ~MySocket_client();

    int init(queue<MSGBODY> * msgQToRecv, queue<MSGBODY> * msgQToSend);

    int connectTo(const char* server_IP, int server_port);// connect
    int setMsg(const char *);
    int getMsg();
    int sendMsg();                 // transfer message
    int fileSend();                // transfer file

    Mylog mylog;
private:

    struct sockaddr_in m_clientAddr;
    CONNECTION m_conn;

    static queue<MSGBODY>  m_msgQueueRecv;  // a queue to storage the msg
    static queue<MSGBODY>  m_msgQueueSend;

    queue<MSGBODY> * mp_msgQueueRecv; // pointer to queue
    queue<MSGBODY> * mp_msgQueueSend;

    int myclose();                  // close socket
};

#endif /* MYSOCKET_CLIENT_H_ */
