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
	u32_t transformX509ToDER(void);
	u32_t transformDERtoX509(void);
	chunk_t getSerialNum(void);
	u32_t getCertType(void);
	s32_t getCertVersion(void);
	chunk_t getSignatureAlgorithmDER(void);
	const s8_t* getSignatureAlgorithmLN(void);
	chunk_t getCertIssuer(void);
	void showCertSubject(void);
	chunk_t getCertSubject(void);
	X509_PUBKEY* getCertPubKey(void);
	EVP_PKEY* getCertPubKey2(void);
	u32_t parseCertExtensions(void);
private:
	X509 *mpX509;
	chunk_t mDerEncoding;   /* DER encoded certificate */
};


#endif

