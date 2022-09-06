#ifndef __CERT_Mgr_H__
#define __CERT_Mgr_H__
#include "mxDef.h"
#include "certX509.h"

class certMgr
{
public:
	certMgr();
	~certMgr();
	certX509* loadX509CertFromPEM(s8_t *filename);
private:
};


#endif

