#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "mxLog.h"
#include "certMgr.h"
#include "asn1Parser.h"





certMgr::certMgr()
{
	
}


certMgr::~certMgr()
{
	certX509 *pCert = NULL;
	while(!mCertList.empty()) {
		pCert = mCertList.front();
		mCertList.pop_front();
		delete pCert;
		pCert = NULL;
	}
}

/**
 * Load a CA certificate from disk
 * X.509 Certification Authority (CA) certificates
 */
certX509* certMgr::loadX509CaCert(s8_t *filename)
{
	u32_t status = 0;
	u32_t encoding = 0;
	status = getCertEncoding(filename, encoding);
	if (status != STATUS_SUCCESS) {
		return NULL;
	}
	certX509 *pCert = new certX509();


	if (encoding == CERT_ENCODING_PEM) {
		pCert->loadX509CertFromPEM(filename);
	} else if (encoding == CERT_ENCODING_DER) {
		pCert->loadX509CertFromDER(filename);
	} else {
		delete pCert;
		return NULL;
	}

	if (!(pCert->getCertFlag() & X509_CA)) {
		delete pCert;
		return NULL;
	}
	mCertList.push_back(pCert);

	chunk_t chunk = pCert->getCertSubjectString();
	mxLogFmt(LOG_DEBUG,"loaded CA certificate %s from %s\n",chunk.ptr, filename);
	chunk_free(&chunk);
	
	return pCert;
}

/**
 * Load a AA certificate from disk
 * X.509 Authorization Authority (AA) certificates
 */
certX509* certMgr::loadX509AaCert(s8_t *filename)
{
	u32_t status = 0;
	u32_t encoding = 0;
	status = getCertEncoding(filename, encoding);
	if (status != STATUS_SUCCESS) {
		return NULL;
	}
	certX509 *pCert = new certX509();


	if (encoding == CERT_ENCODING_PEM) {
		pCert->loadX509CertFromPEM(filename);
	} else if (encoding == CERT_ENCODING_DER) {
		pCert->loadX509CertFromDER(filename);
	} else {
		delete pCert;
		return NULL;
	}

	pCert->setCertFlag(X509_AA);
	mCertList.push_back(pCert);

	chunk_t chunk = pCert->getCertSubjectString();
	mxLogFmt(LOG_DEBUG,"loaded AA certificate %s from %s\n",chunk.ptr, filename);
	chunk_free(&chunk);
	
	return pCert;

}


/**
 * Load a EE certificate from disk
 */
certX509* certMgr::loadX509Cert(s8_t *filename)
{
	u32_t status = 0;
	u32_t encoding = 0;
	status = getCertEncoding(filename, encoding);
	if (status != STATUS_SUCCESS) {
		return NULL;
	}
	certX509 *pCert = new certX509();


	if (encoding == CERT_ENCODING_PEM) {
		pCert->loadX509CertFromPEM(filename);
	} else if (encoding == CERT_ENCODING_DER) {
		pCert->loadX509CertFromDER(filename);
	}
	mCertList.push_back(pCert);

	chunk_t chunk = pCert->getCertSubjectString();
	mxLogFmt(LOG_DEBUG,"loaded EE certificate %s from %s\n",chunk.ptr, filename);
	chunk_free(&chunk);

	return pCert;
}

/**
 * Load a CRL certificate from disk
 */
certX509* certMgr::loadX509CrlCert(s8_t *filename)
{
	certX509 *pCert = new certX509();

	mCertList.push_back(pCert);

	return pCert;
}

/**
 * Load a attribute certificate from disk
 */
certX509* certMgr::loadX509AttCert(s8_t *filename)
{
	certX509 *pCert = new certX509();

	mCertList.push_back(pCert);

	return pCert;
}

/*
.DER .CER，文件是二进制格式，只保存证书，不保存私钥。
.PEM，一般是文本格式，可保存证书，可保存私钥。

*/
u32_t certMgr::getCertEncoding(s8_t *filename,u32_t& encoding)
{
	s32_t fd = 0;
	struct stat sb;
	u32_t ret = 0;
	
	fd = open(filename, O_RDONLY);
	if (fd == -1) {
		return STATUS_FAILED;
	}

	if (fstat(fd, &sb) == -1){
		return STATUS_FAILED;
	}

	s8_t *buf = NULL;
	s32_t len = 0, total = 0;
	buf = (s8_t *)malloc(sb.st_size);
	if (buf== NULL) {
		return STATUS_FAILED;
	}
	while (true) {
		len = read(fd, buf + total, sb.st_size - total);
		if (len < 0) {
			close(fd);
			free(buf);
			return STATUS_FAILED;
		}
		if (len == 0) {
			break;
		}
		total += len;
	}

	if (sb.st_size >= 10 && strncmp(buf,"-----BEGIN", strlen("-----BEGIN")) == 0) {
		encoding = CERT_ENCODING_PEM;
		ret = STATUS_SUCCESS;
	} else {
		chunk_t blob =  {(u8_t *)buf, sb.st_size};
		asn1Parser *parser = new asn1Parser(NULL, {NULL, 0});
		if (parser->isAsn1Encode(blob)) {
			encoding = CERT_ENCODING_DER;
			ret = STATUS_SUCCESS;
		} else {
			ret = STATUS_FAILED;
		}
	}

	close(fd);
	free(buf);
	return ret;
}

list <certX509*> certMgr::getCertList(void)
{
	return mCertList;
}

