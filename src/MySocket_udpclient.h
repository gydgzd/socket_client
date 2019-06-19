/*
 * MySocket_udpclient.h
 *
 *  Created on: May 10, 2019
 *      Author: gyd
 */

#ifndef MYSOCKET_UDPCLIENT_H_
#define MYSOCKET_UDPCLIENT_H_

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


class MySocket_udpclient {
public:
    MySocket_udpclient();
    virtual ~MySocket_udpclient();


private:

};

#endif /* MYSOCKET_UDPCLIENT_H_ */
