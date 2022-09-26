#ifndef __CERT_Mgr_H__
#define __CERT_Mgr_H__
using namespace std;
#include <list>
#include "mxDef.h"
#include "certX509.h"

enum cert_encoding {
	CERT_ENCODING_PEM,
	CERT_ENCODING_DER
};


class certMgr
{
public:
	certMgr();
	~certMgr();
	certX509* loadX509CaCert(s8_t *filename);
	certX509* loadX509AaCert(s8_t *filename);
	certX509* loadX509Cert(s8_t *filename);
	certX509* loadX509CrlCert(s8_t *filename);
	certX509* loadX509AttCert(s8_t *filename);
private:
	u32_t getCertEncoding(s8_t *filename, u32_t& encoding);
	list <certX509*> mCertList;
};


#endif

