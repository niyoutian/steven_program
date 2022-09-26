#include <stdio.h>
#include "mxLog.h"
#include "certX509.h"
#include "credMgr.h"


void test_certX509(void)
{
	certX509 *cert = new certX509();
	mxLogInit2(MXLOG_STDIO, "Test");
	mxLogFmt(LOG_DEBUG,"steven test");

	printf("enter cert\n");
	
	cert->loadX509CertFromPEM("/usr/local/etc/ipsec.d/cacerts/strongswanCert.pem");
	cert->transformX509ToDER();
#if 0
	chunk_t num;
	num = cert->getSerialNum();
	mxLogFmt(LOG_DEBUG,"num.len %d\n",num.len);

	s32_t version = cert->getCertVersion();
	mxLogFmt(LOG_DEBUG,"version %d\n",version);

	chunk_t str = cert->getSignatureAlgorithmDER();
	mxLogFmt(LOG_DEBUG,"str.len %d\n",str.len);

	const s8_t *ln = cert->getSignatureAlgorithmLN();
	mxLogFmt(LOG_DEBUG,"LN:%s\n",ln);
#endif
	cert->getCertIssuer();
	cert->showCertSubject();
	cert->parseCertExtensions();

}


void test_credMgr(void)
{
	credMgr* cred = new credMgr();
	cred->loadCerts();
}

void test_publicKey(void)
{
	certX509 *cert = new certX509();
	cert->loadX509CertFromPEM("/usr/local/etc/ipsec.d/cacerts/strongswanCert.pem");
	chunk_t hash={NULL,0};
	cert->getFingerprint(KEYID_PUBKEY_SHA1, hash);
	chunk_free(&hash);
}


int main(int argc, char* argv[])
{
	//test_certX509();
	test_credMgr();
	//
	test_publicKey();
    return 0;
}






