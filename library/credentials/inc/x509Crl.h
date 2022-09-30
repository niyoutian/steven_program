#ifndef __X509_CRL_H__
#define __X509_CRL_H__
#include <openssl/x509.h>
#include "mxDef.h"
#include "mxStatus.h"
#include "certInterface.h"




class x509Crl : public certInterface
{
public:
	x509Crl();
	~x509Crl();
private:
	X509_CRL *mpCrl;
	chunk_t mDerEncoding;   /* DER encoding of the CRL */
};






#endif

