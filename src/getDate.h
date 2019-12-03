/*
 * getDate.h
 *
 *  Created on: Jul 18, 2016
 *      Author: Gyd
 */
#pragma once
#ifndef GETDATE_H_
#define GETDATE_H_
#ifdef __linux
#include <sys/time.h>
#elif WINVER
#include <Winsock2.h>        // timeval 
#endif

#include <time.h>
#include <stdio.h>
#include <string>
//using namespace std;

//string getSqlTime();

std::string getLocalTime(const char *format);
std::string getLocalTimeUs(const char *format);
time_t dateToSeconds(const char *str);

#endif /* GETDATE_H_ */

/*
class MyTimer
{
public:
	MyTimer();
	~MyTimer();
	int setTimer();
	int checkTimer(int sec_count);
	int reset();
private:
	_int64 m_time;
};*/
