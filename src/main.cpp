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

    queue<DataPacket> msgQueueRecv;
    queue<DataPacket> msgQueueSend;

    sc.init(&msgQueueRecv, &msgQueueSend);
    sc.mylog.logException("****************************BEGIN****************************");
    while (-1 == (ret = sc.connectTo(argv[1], 3401)))
    {
        sleep(5);
    }
    ret = sc.setMsg("world");
    sc.setBuffer();
//    sc.sendMsg();
    int i = 0;
    char msg[10] = "";
    while(true)
    {
        i++;
       sprintf(msg, "%d", i);
        ret = sc.setMsg(msg);
        if(-1 == ret)       // error
            continue;
        usleep(1000);
    //    ret = sc.recvMsg();
        if(ret == -1)
            break;
        sc.sendMsg();
    }
//	fileTransfer(argc, argv);

    return 0;
}


