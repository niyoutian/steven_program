#ifndef __CERT_X509_H__
#define __CERT_X509_H__
#include <openssl/x509.h>
#include "mxDef.h"
#include "mxStatus.h"
#include "certInterface.h"

class certX509 : public certInterface
{
public:
	certX509();
	~certX509();
	u32_t loadX509CertFromPEM(s8_t *filename);
private:
	X509 *mpX509;
};


#endif

