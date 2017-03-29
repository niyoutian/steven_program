#ifndef __MXMYSQL_H__
#define __MXMYSQL_H__

#include <mysql/mysql.h>
#include <string>
#include "mxDef.h"
#include <sstream>

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
	bool executeInsert(string statement);
	bool executeQuery(string statement);
	bool executeUpdate(string statement);
	bool executeDelete(string statement);
	u32_t getAffectRows(void){return mRows;}
	u32_t getFiledNum(void){return mysql_num_fields(mpResult);}
	bool fetchOneRow(void);
	template <typename T>
		T getValueAt(u32_t index);
		
private:
	void freeResultSet(void);
	void setErrorInfo(void);
	MYSQL*			mpConn;
	MYSQL_RES*  	mpResult;
	MYSQL_ROW    	mOneRow;
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

template <>
inline const s8_t* mxMysql::getValueAt(u32_t index)
{
	return mOneRow[index];
}

template <>
inline s8_t* mxMysql::getValueAt(u32_t index)
{
	return mOneRow[index];
}

template <>
inline const u8_t* mxMysql::getValueAt(u32_t index)
{
	return (const u8_t*)mOneRow[index];
}

template <>
inline u8_t* mxMysql::getValueAt(u32_t index)
{
	return (u8_t*)mOneRow[index];
}

template <>
inline const string mxMysql::getValueAt(u32_t index)
{
	return mOneRow[index];
}

template <>
inline string mxMysql::getValueAt(u32_t index)
{
	return mOneRow[index];
}

template <>
inline u32_t mxMysql::getValueAt(u32_t index)
{
	stringstream os;
	os.clear();
	os << mOneRow[index];
	u32_t value;
	os >> value;
	return value;
}

template <>
inline s32_t mxMysql::getValueAt(u32_t index)
{
	stringstream os;
	os.clear();
	os << mOneRow[index];
	s32_t value;
	os >> value;
	return value;
}

template <>
inline u64_t mxMysql::getValueAt(u32_t index)
{
	stringstream os;
	os.clear();
	os << mOneRow[index];
	u64_t value;
	os >> value;
	return value;
}

template <>
inline s64_t mxMysql::getValueAt(u32_t index)
{
	stringstream os;
	os.clear();
	os << mOneRow[index];
	s64_t value;
	os >> value;
	return value;
}


#endif
