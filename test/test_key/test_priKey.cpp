#include <stdio.h>
#include "mxLog.h"
#include "credMgr.h"





void test_priKey01(void)
{
	credMgr* cred = credMgr::getInstance();
	cred->loadSecrets();
}


int main(int argc, char* argv[])
{
	test_priKey01();
    return 0;
}






