#include "certMgr.h"




certMgr::certMgr()
{
	
}


certMgr::~certMgr()
{
	
}

/**
 * Load a CA certificate from disk
 */
certX509* certMgr::loadX509CaCert(s8_t *filename)
{
	certX509 *pCert = new certX509();

	mCertList.push_back(pCert);

	return pCert;
}

/**
 * Load a EE certificate from disk
 */
certX509* certMgr::loadX509Cert(s8_t *filename)
{
	certX509 *pCert = new certX509();

	mCertList.push_back(pCert);

	return pCert;
}

certX509* certMgr::loadX509CertFromPEM(s8_t *filename)
{
	//certX509 * pCert = 

	return NULL;
}

