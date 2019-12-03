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
    int length;            // length of msg
    BYTE msg[MAXLENGTH];

    MSGBODY()
    {
        memset(this, 0, sizeof(MSGBODY));
        type = 0;
        length = 0;
    }
    MSGBODY(const MSGBODY & msgbody)
    {
        memset(this, 0, sizeof(MSGBODY));
        type = msgbody.type;
        length = msgbody.length;
        memcpy(msg,msgbody.msg,length);
    }
};

#define MSGHEAD_LENGTH 80
#define MSGTAIL_LENGTH 4

struct MyFile
{
    char path[128];    // absolute path of the file
    char dstPath[128]; // absolute path to put the file to

};
struct MyMsg           //for send msg to another program
{
    char proto[8] ; // TCP
    int  port;
    char msg[800];  // msg to send
    MyMsg()
    {
        strcpy(proto, "TCP");
        port = 3402;
        memset(msg, 0, sizeof(msg));
    }
    MyMsg(const char *_proto, int _Port, const char* _msg)
    {
        strcpy(proto, _proto);
        port = _Port;
        strcpy(msg, _msg);
    }
};
struct MyCmd
{
    char cmd[256];
};
struct DataPacket
{
    char Head[4];
    char dstNo[32];
    char srcNo[32];
    char type[8];      // file;msg;cmd;
    int  length;       // size
    union
    {
        MyFile myfile;
        MyMsg  mymsg;
        MyCmd  mycmd;
    };
    char Tail[4];

    DataPacket()
    {
        memset(this, 0, sizeof(*this));
        memcpy(Head, "##**", 4);
        memcpy(Tail, "**##", 4);
        length = sizeof(*this) - MSGHEAD_LENGTH - MSGTAIL_LENGTH;
    }
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

    int init(queue<DataPacket> * msgQToRecv, queue<DataPacket> * msgQToSend);
    int setBuffer();
    int connectTo(const char* server_IP, int server_port);// connect
    int reconnect();
    int setMsg(const char *);

    int recvMsg();
    int sendMsg();                 // transfer message
    int fileSend();                // transfer file

    Mylog mylog;
private:
    struct sockaddr_in m_clientAddr;
    CONNECTION m_conn;

    static queue<DataPacket>  m_msgQueueRecv;  // a queue to storage the msg
    static queue<DataPacket>  m_msgQueueSend;

//    queue<MSGBODY> * mp_msgQueueRecv; // pointer to queue
//    queue<MSGBODY> * mp_msgQueueSend;

    queue<DataPacket> * mp_msgQueueRecv; // pointer to queue
    queue<DataPacket> * mp_msgQueueSend;

    int logMsg(const DataPacket *recvMsg, const char *logHead);

    int myclose();                  // close socket
};

#endif /* MYSOCKET_CLIENT_H_ */
