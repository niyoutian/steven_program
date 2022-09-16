#ifndef __CERT_Mgr_H__
#define __CERT_Mgr_H__
using namespace std;
#include <list>
#include "mxDef.h"
#include "certX509.h"

class certMgr
{
public:
	certMgr();
	~certMgr();
	certX509* loadX509CaCert(s8_t *filename);
	certX509* loadX509Cert(s8_t *filename);
private:
	certX509* loadX509CertFromPEM(s8_t *filename);
	list <certX509*> mCertList;
};


#endif

