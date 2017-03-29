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
	/*template <class T>
	 T getValueAt(int index)
	 {
	 	istringstream os(mOneRow[index]); 
		os.clear();
		T num;  
	    os >> num;  
	    return num;
	 }*/
	 template <typename T>
			T getValueAt(u32_t index);
	//template <> u8_t* getValueAt(u32_t index){return (u8_t*)mOneRow[index];}
	//template <> s8_t* getValueAt(u32_t index){return (s8_t*)mOneRow[index];}
		
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
inline u32_t mxMysql::getValueAt(u32_t index)
{
	stringstream os;
	os.clear();
	os << mOneRow[index];
	u32_t value;
	os >> value;
	return value;
}

#endif
