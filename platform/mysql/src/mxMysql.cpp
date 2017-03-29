#include <memory.h>
#include "mxLog.h"
#include "mxMysql.h"



mxMysql::mxMysql():mpConn(NULL),mPort(0),mpResult(NULL),mRows(0),mErrorValue(0)
{
	memset(&mOneRow,0,sizeof(mOneRow));
}

mxMysql::mxMysql(const char* dbName, const char* userName, const char* userPass,
		   const char* hostName, u32_t port) : 
		   mDbName(dbName), mUserName(userName), mUserPass(userPass),
		   mHostName(hostName), mPort(port),mpConn(NULL),mpResult(NULL),mRows(0),mErrorValue(0)
{
	memset(&mOneRow,0,sizeof(mOneRow));
}

mxMysql::~mxMysql()
{
	freeResultSet();
	if(mpConn)
	{
		mysql_close(mpConn);
		mpConn = NULL;	
	}
}

bool mxMysql::openConnection(void)
{
	if (mpConn)
	{
		mxLogFmt(LOG_WARNING,"This object is already connected.");
		return true;
	}
	/*This function indicates whether the client library is compiled as thread-safe.*/
	if ( !mysql_thread_safe() )
	{
		mxLogFmt(LOG_ERR,"the client is not thread-safe");
		return false;
	}

	if( (mpConn = mysql_init(NULL))== NULL)
	{
		mxLogFmt(LOG_ERR,"create mysql handle failed");
		return false;
	}
	/** Enable Reconnect */
	const my_bool *option = "1";
	mysql_options(mpConn, MYSQL_OPT_RECONNECT, option);
	
	unsigned long opt_flags = CLIENT_FOUND_ROWS 	  |
							CLIENT_MULTI_STATEMENTS |
							CLIENT_MULTI_RESULTS	;

	if (NULL == mysql_real_connect(	mpConn,
								   	mHostName.empty() ? NULL : mHostName.c_str(),
								   	mUserName.empty() ? NULL : mUserName.c_str(),
								   	mUserPass.empty() ? NULL : mUserPass.c_str(),
								   	mDbName.empty()   ? NULL : mDbName.c_str(),
								   	mPort,0,opt_flags))
	{
		mysql_close(mpConn);
		mpConn = NULL;
		mxLogFmt(LOG_ERR,"could not connect to DB %s Host %s User %s Pass %s port %u",
				mDbName.empty()   ? "NULL" : mDbName.c_str(),
				mHostName.empty() ? "NULL" : mHostName.c_str(),
				mUserName.empty() ? "NULL" : mUserName.c_str(),
				mUserPass.empty() ? "NULL" : mUserPass.c_str(),
				mPort);
		return false;
	}
	mxLogFmt(LOG_INFO,"connect to DB %s Host %s User %s Pass %s port %u successful",
				mDbName.empty()   ? "NULL" : mDbName.c_str(),
				mHostName.empty() ? "NULL" : mHostName.c_str(),
				mUserName.empty() ? "NULL" : mUserName.c_str(),
				mUserPass.empty() ? "NULL" : mUserPass.c_str(),
				mPort);	
	const char * client_ver = mysql_get_client_info();
	const char * server_ver = mysql_get_server_info(mpConn);
	mxLogFmt(LOG_INFO,"mysql client version=%s, server version = %s",client_ver,server_ver);
	mxLogFmt(LOG_INFO,"mysql host information %s",mysql_get_host_info(mpConn));
	return true;
}

bool mxMysql::isConnected(void)
{
	if(mpConn == NULL)
		return false;
	return (mysql_ping(mpConn) == 0) ? true : false;
}

bool mxMysql::executeInsert(string statement)
{
	/*reset the statement before every call*/
	freeResultSet();
	if(mysql_real_query(mpConn,statement.c_str(),statement.length()))
	{
		setErrorInfo();
		return false;
	}
	mRows = mysql_affected_rows(mpConn);
	return true;
}

bool mxMysql::executeQuery(string statement)
{
	/*reset the statement before every call*/
	freeResultSet();
	if(mysql_real_query(mpConn,statement.c_str(),statement.length()))
	{
		setErrorInfo();
		return false;
	}
	mpResult = mysql_store_result(mpConn);
	if(mpResult == NULL)
	{
		setErrorInfo();
		return false;
	}
	mRows = mysql_affected_rows(mpConn);
	return true;
}

bool mxMysql::executeUpdate(string statement)
{
	/*reset the statement before every call*/
	freeResultSet();
	if(mysql_real_query(mpConn,statement.c_str(),statement.length()))
	{
		setErrorInfo();
		return false;
	}
	mRows = mysql_affected_rows(mpConn);
	return true;
}

bool mxMysql::executeDelete(string statement)
{
	/*reset the statement before every call*/
	freeResultSet();
	if(mysql_real_query(mpConn,statement.c_str(),statement.length()))
	{
		setErrorInfo();
		return false;
	}
	mRows = mysql_affected_rows(mpConn);
	return true;
}

bool mxMysql::fetchOneRow(void)
{
	return mpResult ? ((mOneRow = mysql_fetch_row(mpResult)) != NULL) : false;
}


void mxMysql::freeResultSet(void)
{
	if (mpConn)
	{
		while ((mysql_next_result(mpConn)) == 0); //all result sets skipped
	}

	if (mpResult)
	{
		mxLogFmt(LOG_INFO, "freeing result set...");
		mysql_free_result(mpResult);
		mpResult = NULL;
	}	
}

void mxMysql::setErrorInfo(void)
{
	mErrorValue = mysql_errno(mpConn);
	mSqlState   = mysql_sqlstate(mpConn);
	mErrorInfo  = mysql_error(mpConn);
}

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
#if 0
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


