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

    memset(&m_clientAddr, 0, sizeof(m_clientAddr));
    m_clientAddr.sin_family = AF_INET;

    mp_msgQueueRecv = msgQToRecv;
    mp_msgQueueSend = msgQToSend;
    if( (mn_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        sprintf(logmsg, "ERROR: Create socket error: %s(errno: %d)\n", strerror(errno), errno);
        mylog.logException(logmsg);
        exit(-1);
    }
    return 0;
}
int MySocket_client::setBuffer()
{
    // get buffer
    int s_length, r_length;
    socklen_t optl = sizeof(s_length);
    getsockopt(m_conn.socket_fd,SOL_SOCKET,SO_SNDBUF,&s_length,&optl);     //锟斤拷锟斤拷锟斤拷锟斤拷捉锟斤拷址锟斤拷投嘶锟斤拷锟斤拷锟斤拷锟斤拷锟较�
    getsockopt(m_conn.socket_fd,SOL_SOCKET,SO_RCVBUF,&r_length,&optl);     //锟斤拷锟斤拷锟斤拷锟斤拷捉锟斤拷值慕锟斤拷斩说幕锟斤拷锟斤拷锟斤拷锟较�
    printf("send buffer = %d, recv buffer = %d\n",s_length, r_length);
 /* */

    // set buffer
/*  int nRecvBufSize = 64*1024;//锟斤拷锟斤拷为64K
    setsockopt(m_conn.socket_fd,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBufSize,sizeof(int));
    int nSendBufSize = 64*1024;//锟斤拷锟斤拷为64K
    setsockopt(m_conn.socket_fd,SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBufSize,sizeof(int));
*/
    return 0;
}
/*
 * connect to a server
 */
int MySocket_client::connectTo(const char* server_IP, int server_port)
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
    // set nonlocking mode
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
    // set nonlocking mode
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
    MSGBODY mymsg;
    mymsg.type = 1;
    memset(mymsg.msg, 0, sizeof(mymsg));
    int length = strlen(str);
    memcpy(mymsg.msg, str, length);
    mymsg.length = length;
    mp_msgQueueSend->push(mymsg);
    return 0;
}
int MySocket_client::recvMsg()
{
    MSGBODY recvMsg;
    recvMsg.length = 0;
    char logmsg[512] = "";
    char logHead[64] = "";
    sprintf(logHead, "%s:%d --> %s:%d ", m_conn.clientIP, m_conn.clientPort, m_conn.serverIP, m_conn.serverPort);
    int length = 0;
    char * p_hexLog = NULL;
    // recv head, to get the length of msg
    length = recv(mn_socket_fd, &recvMsg, MSGHEAD_LENGTH, 0);
    if(length == -1)     // recv
    {
        if(errno != 11) // data isnot ready when errno = 11, log other error
        {
           int err = errno;
           sprintf(logmsg, "ERROR: %s: recv error: %d--%s",logHead, errno, strerror(errno) );
           mylog.logException(logmsg);
            if(err == 9)
            {
                close(mn_socket_fd);
                reconnect();
                return 0;
            }
        }
        usleep(10000);
        return 0;
    }
    else                     // recv success
    {
       if(length == 0)
       {
           close(mn_socket_fd);
           sprintf(logmsg, "INFO: %s: Process exit.\n", logHead);
           mylog.logException(logmsg);
           return 0;
       }
    }
    // recv head, to get the length of msg
    if(0 != recvMsg.length)
    {
        if(recvMsg.type != 0 && recvMsg.type != 1 && recvMsg.type != 2)
            return -1;
        printf("type = %d, recvLen = %d,\n", recvMsg.type, recvMsg.length);
        length = recv(mn_socket_fd, recvMsg.msg, recvMsg.length, 0);
        if(length == -1)
        {
            if(errno != 11) // data isnot ready when errno = 11, log other error
            {
                sprintf(logmsg, "ERROR: %s recv msg error: %d--%s",logHead, errno, strerror(errno) );
                mylog.logException(logmsg);
            }
            if(errno == 9)
            {
                close(mn_socket_fd);
                mylog.logException("ERROR: recv exit.");
                return 0;
            }
            //sleep(1);
            usleep(10000);  // 10ms
            length = 0;  // set it back to 0
            return -1;
        }
        else
        {
            if( length == 0 )
            {
                close(mn_socket_fd);
                mylog.logException("ERROR: recv exit.");
                return 0;
            }
            else
            {
                mp_msgQueueRecv->push(recvMsg);
                if(recvMsg.type == 0)
                {
                    char logmsg[1024];
                    sprintf(logmsg, "INFO: %s recved: %d",logHead, recvMsg.msg);
                    mylog.logException(logmsg);
                }
                else if(recvMsg.type == 1)
                {
                    char logmsg[1024];
                    sprintf(logmsg, "INFO: %s recved: %s",logHead, recvMsg.msg);
                    mylog.logException(logmsg);
                }
                else if(recvMsg.type == 2)
                {
                    try
                    {
                        p_hexLog = new char[recvMsg.length*3 + 128];    // include the logHead
                        memset(p_hexLog, 0, recvMsg.length*3 + 128);
                        sprintf(p_hexLog, "INFO: %s recved: ", logHead);
                        int len = strlen(p_hexLog);
                        for(int i=0; i<recvMsg.length; i++)
                            sprintf(p_hexLog+len+3*i, "%02x ", (unsigned char)recvMsg.msg[i]);
                        mylog.logException(p_hexLog);
                        delete[] p_hexLog;
                    }
                    catch(bad_alloc& bad)
                    {
                       sprintf(logmsg,"ERROR: Failed to alloc mem when log hex: %s", bad.what());
                       mylog.logException(logmsg);
                    }
                }
            }
        }
    }

    memset(recvMsg.msg, 0, recvMsg.length);
    return 0;
}
int MySocket_client::sendMsg()
{
    char logmsg[512] = "";
    char logHead[64] = "";
    sprintf(logHead, "%s:%d --> %s:%d ", m_conn.clientIP, m_conn.clientPort, m_conn.serverIP, m_conn.serverPort);
    // send
    if(mp_msgQueueSend->empty())
    {
        sleep(1);
        return 0;
    }
    int sendLen = sizeof(mp_msgQueueSend->front().length) + sizeof(mp_msgQueueSend->front().type) + mp_msgQueueSend->front().length;
    if( send(mn_socket_fd, &mp_msgQueueSend->front(), sendLen, 0) < 0)
    {
        int err = errno;
        sprintf(logmsg, "ERROR: %s: send msg error: %s(errno: %d)", logHead, strerror(errno), errno);
        mylog.logException(logmsg);
        if(err == EPIPE || err == 104 || err == 11 || err == 9 )
        {
            close(mn_socket_fd);
            reconnect();
        }
    }
    else if(mp_msgQueueSend->front().type == 1)
    {
        sprintf(logmsg, "INFO: %s: send: %s", logHead, mp_msgQueueSend->front().msg);
        mylog.logException(logmsg);
    }
    mp_msgQueueSend->pop();
    return 0;
}
/*
 * log Msg
 */
int MySocket_client::logMsg(const MSGBODY *recvMsg, const char *logHead)
{
    char logmsg[256] = "";
    if(2 == recvMsg->type)             //  hex
    {
        try
        {
            char *p_hexLog = new char[recvMsg->length*3 + 128];    // include the logHead
            memset(p_hexLog, 0, recvMsg->length*3 + 128);
            sprintf(p_hexLog, "INFO: %s recved: ", logHead);
            int len = strlen(p_hexLog);
            for(int i=0; i<recvMsg->length; i++)
                sprintf(p_hexLog+len+3*i, "%02x ", (unsigned char)recvMsg->msg[i]);
            mylog.logException(p_hexLog);
            delete[] p_hexLog;
        }catch(bad_alloc& bad)
        {
            sprintf(logmsg,"ERROR: Failed to alloc mem when log hex: %s", bad.what());
            mylog.logException(logmsg);
        }
    }
    else if(1 == recvMsg->type)
    {
        char logmsg[recvMsg->length + 128];
        memset(logmsg, 0, recvMsg->length + 128);
        sprintf(logmsg, "INFO: %s recved: %s", logHead, recvMsg->msg);
        mylog.logException(logmsg);
    }
    return 0;
}
