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
    while (-1 == (ret = sc.myconnect("172.18.10.129", 3402)))
    {
        sleep(6);
    }
    while(true)
    {
        ret = sc.setMsg();
        if(-1 == ret)       // error
            continue;

        ret = sc.sendMsg();
        if(ret == -1)
            break;
    }
//	fileTransfer(argc, argv);

    return 0;
}


