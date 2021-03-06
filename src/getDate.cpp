#include "getDate.h"

//#include "sql_conn_cpp.h"  //my sql class

/* get time from mysql server
string getSqlTime()
{
	string date_str;
	string dbname = "websiteman";
	string sql = "select date_format(now(),'%Y-%m-%d %H:%i:%s') time";    // date_format(now(6),'%Y-%m-%d %H:%i:%s %f')
	try
	{
	   Csql_conn_cpp  sqlcon;
	   sqlcon.sql_connect("10.1.122.140");
	   sqlcon.setDBName(dbname);
	   sqlcon.sql_execute(sql,1);  //put result into ss.res-> and there might be an exception here either
	   if (sqlcon.m_res->next())
	   {
		   date_str = sqlcon.m_res->getString("time").c_str();  //getString()
	   }
	}
	catch(sql::SQLException &e)
	{
		date_str = getLocalTime("%Y-%m-%d %H:%M:%S");    // get local time when failed
	}
	return date_str;
}
*/
/* 
return the local time, like 
"%Y-%m-%d %H:%M:%S", 
"%Y-%m-%d", 
"%Y" 
*/
std::string getLocalTime(const char *format)
{
	time_t t = time(0);
	std::string date_str = "";
	char tmp[32] = {};

#ifdef __linux
	strftime(tmp, sizeof(tmp), format, localtime(&t));
#elif WINVER
	tm timeinfo;
	localtime_s(&timeinfo, &t);
	strftime(tmp, sizeof(tmp), format, &timeinfo);
#endif

	date_str = tmp;
	//puts( tmp );
	return date_str;
}
/*
return the local time, like
"%Y-%m-%d %H:%M:%S",
"%Y-%m-%d",
"%Y"
*/
std::string getLocalTimeUs(const char *format)
{
    struct timeval tv;
    gettimeofday(&tv,NULL);

    std::string date_str = "";
    char tmp[32] = {};

#ifdef __linux
    strftime(tmp, sizeof(tmp), format, localtime(&tv.tv_sec));
#elif WINVER
    tm timeinfo;
    localtime_s(&timeinfo, &tv.tv_sec);
    strftime(tmp, sizeof(tmp), format, &timeinfo);
#endif

    date_str = tmp;
    sprintf(tmp, " %06ld", tv.tv_usec);
    date_str += tmp;
    return date_str;
}
/*
 * change the string format date into seconds
 * return the seconds
 */
time_t dateToSeconds(const char *str)
{
    tm timeinfo;
    int year, month, day, hour, minute,second;
    sscanf(str,"%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    timeinfo.tm_year  = year-1900;
    timeinfo.tm_mon   = month-1;
    timeinfo.tm_mday  = day;
    timeinfo.tm_hour  = hour;
    timeinfo.tm_min   = minute;
    timeinfo.tm_sec   = second;
    timeinfo.tm_isdst = 0;

    time_t t_ = mktime(&timeinfo); //已经减了8个时区
    return t_; //秒时间
}
