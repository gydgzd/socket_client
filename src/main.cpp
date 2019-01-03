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
	sc.connectTo("172.18.10.129", 3401);
	int ret = 0;

	while(true)
	{
		sc.getMsg();
//		sc.setMsg("\n");
		ret = sc.sendMsg();
		if(ret == -1)
			break;
	}
//	fileTransfer(argc, argv);

	return 0;
}


