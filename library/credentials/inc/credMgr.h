#ifndef __CRED_MGR_H__
#define __CRED_MGR_H__
#include "mxDef.h"
#include "certMgr.h"


class credMgr
{
public:
	credMgr();
	~credMgr();
	//void loadX509Cert(void);
	//certX509* loadX509CertFromPEM(void);
private:
	certMgr *mpCertMgr;
};

#endif

