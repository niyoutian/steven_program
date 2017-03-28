#include "test_mysql.h"
#include "mxLog.h"
#include "mxMysql.h"











int main(int argc, char* argv[])
{
	mxLogInit("mysql");
	mxLogFmt(LOG_DEBUG,"mysql test");
#if 1
	mxMysql *mysql = new mxMysql("financial_manager_data","root","steven");
	if(mysql)
		mysql->openConnection();
	bool ret = mysql->executeQuery("select *from financialInfo;");
	mxLogFmt(LOG_DEBUG,"ret %d rows %d filed %d",ret,mysql->getAffectRows(),mysql->getFiledNum());
#endif
	return 0;
}






