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
    sc.init(&msgQueueRecv, &msgQueueSend);
    sc.mylog.logException("****************************BEGIN****************************");
    while (-1 == (ret = sc.connectTo("172.18.10.11", 3401)))
    {
        sleep(5);
    }
    while(true)
    {
        ret = sc.getMsg();
        if(-1 == ret)       // error
            continue;
//		sc.setMsg("\n");
        ret = sc.sendMsg();
        if(ret == -1)
            break;
    }
//	fileTransfer(argc, argv);

    return 0;
}


