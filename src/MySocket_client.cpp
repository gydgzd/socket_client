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
    mp_msgQueueRecv = NULL;
    mp_msgQueueSend = NULL;
    mylog.setMaxFileSize(200*1024*1024);      // 200MB

//    init(&m_msgQueueRecv, &m_msgQueueRecv);
}

MySocket_client::~MySocket_client()
{
    close(mn_socket_fd);
}

/*
 * initialize to use outside queue,
 * if not specified, use member queue
 */
int MySocket_client::init(queue<MSGBODY> * msgQToRecv = &m_msgQueueRecv, queue<MSGBODY> * msgQToSend = &m_msgQueueSend)
{
    char logmsg[512] = "";
    mylog.logException("****************************BEGIN****************************");
    if( (mn_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        sprintf(logmsg, "ERROR: Create socket error: %s(errno: %d)\n", strerror(errno), errno);
        mylog.logException(logmsg);
        exit(-1);
    }
    memset(&m_clientAddr, 0, sizeof(m_clientAddr));
    m_clientAddr.sin_family = AF_INET;

    mp_msgQueueRecv = msgQToRecv;
    mp_msgQueueSend = msgQToSend;

    return 0;
}
/*
 * connect to a server by IP
 */
int MySocket_client::myconnect(const char* server_IP, int server_port)
{
    char logmsg[512] = "";
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

int MySocket_client::reconnect()
{
    char logmsg[512] = "";
    if( (mn_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        sprintf(logmsg, "ERROR: Create socket error: %s(errno: %d)\n", strerror(errno), errno);
        mylog.logException(logmsg);
        exit(-1);
    }
    while( connect(mn_socket_fd, (struct sockaddr*)&m_clientAddr, sizeof(m_clientAddr)) < 0)
    {
        sprintf(logmsg, "ERROR: reconnect error: %s(errno: %d)\n", strerror(errno), errno);
        mylog.logException(logmsg);
        sleep(6);
    }
    mylog.logException("INFO: reconnect successfully.");
    return 0;
}
int MySocket_client::setMsg(const char *str)
{
    MSGBODY mymsg;
    mymsg.type = 1;
    memset(mymsg.msg, 0, sizeof(mymsg));
    int length = strlen(str);
    memcpy(mymsg.msg, str, length);
    mymsg.length = length;
    mp_msgQueueSend->push(mymsg);
    return 0;
}
// get msg from keyboard
int MySocket_client::setMsg()
{
    MSGBODY mymsg;
    memset(mymsg.msg, 0, sizeof(mymsg));

//    char *s = fgets((char *)mymsg.msg, MAXLENGTH, stdin);
    mymsg.length = sprintf((char *)mymsg.msg, "world");
    mymsg.type = 1;
    mp_msgQueueSend->push(mymsg);
    return 0;
}

int MySocket_client::sendMsg()
{
    MSGBODY recvBuf;
    recvBuf.length = 0;
    char logmsg[512] = "";
    char logHead[64] = "";
    sprintf(logHead, "%s:%d --> %s:%d ", m_conn.clientIP, m_conn.clientPort, m_conn.serverIP, m_conn.serverPort);
    // send
    int sendLen = sizeof(mp_msgQueueSend->front().length) + sizeof(mp_msgQueueSend->front().type) + mp_msgQueueSend->front().length;
    if( send(mn_socket_fd, &mp_msgQueueSend->front(), sendLen, 0) < 0)
    {
        sprintf(logmsg, "ERROR: %s: send msg error: %s(errno: %d)", logHead, strerror(errno), errno);
        mylog.logException(logmsg);
        close(mn_socket_fd);
        reconnect();
    }
    if(mp_msgQueueSend->front().type == 1)
    {
        sprintf(logmsg, "INFO: %s: send: %s", logHead, mp_msgQueueSend->front().msg);
        mylog.logException(logmsg);
    }

    mp_msgQueueSend->pop();
    // recv
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
        if(strcmp((char *)recvBuf.msg,"exit\n")==0 || recvBuf.length == 0)
        {
            close(mn_socket_fd);
            mylog.logException("INFO: Remote server disconnected. Exit.");
            reconnect();
        }
        mp_msgQueueRecv->push(recvBuf);
        try
        {
            if(recvBuf.type == 1)
            {
            char logmsg[1024];
            sprintf(logmsg, "INFO: %s recved: %s",logHead, recvBuf.msg);
            mylog.logException(logmsg);
            }
            else if(recvBuf.type == 2)
            {
            p_hexLog = new char[recvBuf.length*3 + 128];    // include the logHead
            memset(p_hexLog, 0, recvBuf.length*3 + 128);
            sprintf(p_hexLog, "INFO: %s recved: ", logHead);
            int len = strlen(p_hexLog);
            for(int i=0; i<recvBuf.length; i++)
                sprintf(p_hexLog+len+3*i, "%02x ", (unsigned char)recvBuf.msg[i]);
            mylog.logException(p_hexLog);
            delete[] p_hexLog;
            }
        }
        catch(bad_alloc& bad)
        {
            sprintf(logmsg,"ERROR: Failed to alloc mem when log hex: %s", bad.what());
            mylog.logException(logmsg);
		}
    }
    memset(recvBuf.msg, 0, recvBuf.length);
    return 0;
}
