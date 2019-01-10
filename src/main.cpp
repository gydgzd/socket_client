/*
 * main.cpp
 *
 *  Created on: Apr 23, 2017
 *      Author: gyd
 */


#include <iostream>
using namespace std;
#include "MySocket_client.h"

extern int fileTransfer(int argc, char **argv);
extern int socket_client(char* server_IP);
int main(int argc, char **argv)
{
    MySocket_client sc;
    int ret = 0;

    queue<MSGBODY> msgQueueRecv;
    queue<MSGBODY> msgQueueSend;
    sc.init(&msgQueueSend, &msgQueueSend);
    while (-1 == (ret = sc.myconnect("172.18.10.129", 33402)))
    {
        sleep(6);
    }
    ret = sc.setMsg("world");
    sc.setBuffer();
    sc.sendMsg();
    while(true)
    {
   //     ret = sc.setMsg("world");
        if(-1 == ret)       // error
            continue;
        usleep(10000);
        ret = sc.recvMsg();
        if(ret == -1)
            break;
        sc.sendMsg();
    }
//	fileTransfer(argc, argv);

    return 0;
}


