#include "mxLog.h"
#include "mxMysql.h"



mxMysql::mxMysql():mpConn(NULL),mPort(0)
{
	
}

mxMysql::mxMysql(const char* dbName, const char* userName, const char* userPass,
		   const char* hostName, u32_t port) : 
		   mDbName(dbName), mUserName(userName), mUserPass(userPass),
		   mHostName(hostName), mPort(port),mpConn(NULL)
{
	
}

mxMysql::~mxMysql()
{
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
	if (NULL == mysql_real_connect(	mpConn,
								   	mHostName.empty() ? NULL : mHostName.c_str(),
								   	mUserName.empty() ? NULL : mUserName.c_str(),
								   	mUserPass.empty() ? NULL : mUserPass.c_str(),
								   	mDbName.empty()   ? NULL : mDbName.c_str(),
								   	mPort,0,0))
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

bool mxMysql::executeQuery(string statement)
{
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

void mxMysql::setErrorInfo(void)
{
	mErrorValue = mysql_errno(mpConn);
	mSqlState   = mysql_sqlstate(mpConn);
	mErrorInfo  = mysql_error(mpConn);
}

