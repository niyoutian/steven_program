#include "test_mysql.h"
#include "mxLog.h"
#include "mxMysql.h"











int main(int argc, char* argv[])
{
	mxLogInit("mysql");
	mxLogFmt(LOG_DEBUG,"mysql test");
	mxMysql *mysql = new mxMysql("ikeDb","root","");
	if(mysql)
		mysql->openConnection();
	bool ret = mysql->executeQuery("select *from ikeSa;");
	mxLogFmt(LOG_DEBUG,"ret %d",ret);
	return 0;
}






