/*
 * main.cpp
 *
 *  Created on: Apr 23, 2017
 *      Author: gyd
 */


#include <iostream>
using namespace std;
#include "MySocket_client.h"
#include "md5.h"
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
    char msg[] = "1. We will continue to develop new and improved approaches to macro regulation and keep the main economic indicators within an appropriate range. We will keep using market-oriented reform thinking and approaches to resolve problems in development; make good use of countercyclical macro policy regulation; add to and use flexibly financial, monetary, and employment policy instruments; make regulation more forward-looking, targeted, and effective; and create the conditions for ensuring a stable economic performance."
"We will implement larger-scale tax cuts. We will introduce both general-benefit and structural tax cuts, focusing primarily on reducing tax burdens in manufacturing and on small and micro businesses."
"2. VAT reform will be deepened: we will reduce the current rate of 16 percent in manufacturing and other industries to 13 percent, and lower the rate in the transportation, construction, and other industries from 10 to 9 percent, thus ensuring that tax burdens in our main industries are meaningfully reduced; keep the lowest bracket rate unchanged at 6 percent, while adopting supporting measures, like increased tax deductions for producer and consumer services, to see that in all industries tax burdens only go down, not up; and continue making progress toward cutting the number of VAT brackets from three to two and simplifying the VAT system. "
"3. We will ensure that the general-benefit tax cut policies issued at the start of the year for small and micro businesses are put into effect. Our moves to cut tax on this occasion aim at an accommodative effect to strengthen the basis for sustained growth while also considering the need to ensure fiscal sustainability; are a major measure to lighten the burden on businesses and boost market dynamism; are an important reform to improve the tax system and achieve better income distribution; and are the result of a major decision taken at the macro policy level in support of the efforts to ensure stable economic growth, employment, and structural adjustments."
;
    cout << md5(msg) << endl;
    cout << md5("nice") << endl;
    MD5 md;
    cout << md.fileMD5("/home/gyd/project/c/ServerProbe/java")<<endl;;
    ret = sc.setMsg(msg);
    sc.setBuffer();
//    sc.sendMsg();
    int i = 0;
//    char msg[10] = "";
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


