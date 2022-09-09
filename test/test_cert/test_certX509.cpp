#include <stdio.h>
#include "mxLog.h"
#include "certX509.h"





int main(int argc, char* argv[])
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
    return 0;
}






