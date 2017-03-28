#ifndef __MXMYSQL_H__
#define __MXMYSQL_H__

#include <mysql/mysql.h>
#include <string>
#include "mxDef.h"
using namespace std;

class mxMysql
{
public:
	mxMysql();
	mxMysql(const char* dbName, const char* userName, const char* userPass,
			   const char* hostName = "127.0.0.1", u32_t port = 3306);
	~mxMysql();
	bool openConnection(void);
	bool isConnected(void);
	bool executeQuery(string statement);
private:
	void setErrorInfo(void);
	MYSQL*			mpConn;
	MYSQL_RES*  	mpResult;
	my_ulonglong	mRows;
	string  		mHostName;
	string			mUserName;
	string			mUserPass;
	string			mDbName;
	u32_t       	mPort;

	u32_t 			mErrorValue;
	string 			mErrorInfo;
	string   		mSqlState;

};


#endif
