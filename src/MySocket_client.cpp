/*
 * mysocket_client.cpp
 *
 *  Created on: Jun 7, 2017
 *      Author: gyd
 */
#include "MySocket_client.h"



MySocket_client::MySocket_client()
{
	mn_socket_fd = 0;
	mq_msgQueueRecv = NULL;
	mq_msgQueueSend = NULL;
	mylog.setMaxFileSize(200*1024*1024); // 200MB

	init();
}

MySocket_client::~MySocket_client()
{
	close(mn_socket_fd);
}

int MySocket_client::init()
{
	if( (mn_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Create socket error: %s(errno: %d)\n", strerror(errno), errno);
		exit(-1);
	}
	memset(&m_clientAddr, 0, sizeof(m_clientAddr));
	m_clientAddr.sin_family = AF_INET;

	memset(m_recvBuf.msg, 0, MAXLENGTH);
	memset(m_sendBuf.msg, 0, MAXLENGTH);

	return 0;
}
/*
 * connect to a server by IP
 */
int MySocket_client::connectTo(const char* server_IP, int server_port)
{
	char logmsg[512] = "";
	mylog.logException("****************************BEGIN****************************");
	if( inet_pton(AF_INET, server_IP, &m_clientAddr.sin_addr) <= 0)
	{
		sprintf(logmsg, "ERROR: connectTo %s error, inet_pton error: %s\n", server_IP, strerror(errno));
		mylog.logException(logmsg);
		return -1;
	}
	m_clientAddr.sin_port = htons(server_port);
	if( connect(mn_socket_fd, (struct sockaddr*)&m_clientAddr, sizeof(m_clientAddr)) < 0)
	{
		sprintf(logmsg, "ERROR: connectTo %s:%d error: %s(errno: %d)\n", server_IP, server_port, strerror(errno), errno);
		mylog.logException(logmsg);
		return -1;
	}

	// connect success
	// get server address
	memset(&m_conn, 0, sizeof(m_conn));
	memcpy(m_conn.serverIP, server_IP, strlen(server_IP));
	m_conn.serverPort = server_port;

	// get client address
	m_conn.socket_fd = mn_socket_fd;
	struct sockaddr_in local_addr;
	socklen_t local_len = sizeof(local_addr);
	memset(&local_addr, 0, local_len);
	getsockname(mn_socket_fd, (struct sockaddr *)&local_addr, &local_len);
	inet_ntop(AF_INET,(void *)&local_addr.sin_addr, m_conn.clientIP, 64 );
	m_conn.clientPort = ntohs(local_addr.sin_port);

	sprintf(logmsg, "INFO: %s:%d --> %s:%d connected", m_conn.clientIP, m_conn.clientPort, m_conn.serverIP, m_conn.serverPort);
	mylog.logException(logmsg);
	// 使用非阻塞io
	int flags;
	if( (flags = fcntl(mn_socket_fd, F_GETFL, 0)) < 0)
	{
		sprintf(logmsg, "ERROR: fcntl error: %d--%s", errno, strerror(errno) );
		mylog.logException(logmsg);
		return -1;
	}
	fcntl(mn_socket_fd, F_SETFL, flags | O_NONBLOCK);
	return 0;
}
int MySocket_client::setMsg(const char *str)
{
	int length = strlen(str);
	memcpy(m_sendBuf.msg, str, length);
	return 0;
}
int MySocket_client::getMsg()
{
	memset(m_sendBuf.msg, 0, MAXLENGTH);
	fgets((char *)m_sendBuf.msg, MAXLENGTH, stdin);
	return 0;
}

int MySocket_client::sendMsg()
{
	MSGBODY recvBuf;
	recvBuf.length = 0;
	char logmsg[512] = "";
	char logHead[64] = "";
	sprintf(logHead, "%s:%d --> %s:%d ", m_conn.clientIP, m_conn.clientPort, m_conn.serverIP, m_conn.serverPort);
	//
	if( send(mn_socket_fd, recvBuf.msg, recvBuf.length, 0) < 0)
	{
		sprintf(logmsg, "ERROR: %s: send msg error: %s(errno: %d)", logHead, strerror(errno), errno);
		mylog.logException(logmsg);
		close(mn_socket_fd);
		exit(-1);
	}
	sprintf(logmsg, "INFO: %s: send: %s", logHead, recvBuf.msg);
	mylog.logException(logmsg);

	char * p_hexLog = NULL;
	if((recvBuf.length = recv(mn_socket_fd, recvBuf.msg, MAXLINE, 0)) == -1)
	{
		// data isnot ready when errno = 11
		if(errno != 11)
		{
			sprintf(logmsg, "ERROR: %s: recv error: %d--%s",logHead, errno, strerror(errno) );
			mylog.logException(logmsg);
		}
		//sleep(1);
		usleep(10000);  // 10ms
		recvBuf.length = 0;  // set it back to 0
	}
	else
	{
			try
		{
			p_hexLog = new char[recvBuf.length*3 + 128];    // include the logHead
			memset(p_hexLog, 0, recvBuf.length*3 + 128);
			sprintf(p_hexLog, "INFO: %s recved: ", logHead);
			int len = strlen(p_hexLog);
			for(int i=0; i<recvBuf.length; i++)
				sprintf(p_hexLog+len+3*i, "%02x ", (unsigned char)recvBuf.msg[i]);
			mylog.logException(p_hexLog);
			delete[] p_hexLog;
		}catch(bad_alloc& bad)
		{
			sprintf(logmsg,"ERROR: Failed to alloc mem when log hex: %s", bad.what());
			mylog.logException(logmsg);
		}

		if(strcmp((char *)recvBuf.msg,"exit\n")==0)
		{
			close(mn_socket_fd);
			return -1;
		}
	}
	memset(recvBuf.msg, 0, recvBuf.length);
	memset(recvBuf.msg, 0, recvBuf.length);

	return 0;
}
