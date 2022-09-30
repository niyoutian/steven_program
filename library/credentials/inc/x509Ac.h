#ifndef __X509_CRL_H__
#define __X509_CRL_H__
#include <openssl/x509.h>
#include "mxDef.h"
#include "mxStatus.h"
#include "certInterface.h"




class x509Ac : public certInterface
{
public:
	x509Ac();
	~x509Ac();
private:
};






#endif

