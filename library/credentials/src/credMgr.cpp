#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include "mxStatus.h"
#include "mxLog.h"
#include "credMgr.h"


credMgr::credMgr()
{
	mpCertMgr = new certMgr();
}

credMgr::~credMgr()
{
	if (mpCertMgr) {
		delete mpCertMgr;
	}
}

/**
 * load all certificates
 */
void credMgr::loadCerts(void)
{
	mxLogFmt(LOG_INFO,"loading ca certificates from '%s'\n",CA_CERTIFICATE_DIR);
	loadCertDir(CA_CERTIFICATE_DIR, CERT_X509, X509_CA);
	
	mxLogFmt(LOG_INFO,"loading aa certificates from '%s'\n",AA_CERTIFICATE_DIR);
	loadCertDir(AA_CERTIFICATE_DIR, CERT_X509, X509_AA);
	
	mxLogFmt(LOG_INFO,"loading ocsp signer certificates from '%s'\n",OCSP_CERTIFICATE_DIR);
	loadCertDir(OCSP_CERTIFICATE_DIR, CERT_X509, X509_OCSP_SIGNER);
	
	mxLogFmt(LOG_INFO,"loading attribute certificates from '%s'\n",ATTR_CERTIFICATE_DIR);
	loadCertDir(ATTR_CERTIFICATE_DIR, CERT_X509_AC, 0);

	mxLogFmt(LOG_INFO,"loading crls certificates from '%s'\n",CRL_DIR);
	loadCertDir(CRL_DIR, CERT_X509_CRL, 0);
}

certMgr* credMgr::getCertMgr(void)
{
	return mpCertMgr;
}


/**
 * load trusted certificates from a directory
 */
u32_t credMgr::loadCertDir(s8_t *path, u32_t certType, u32_t certFlag)
{
	/** directory handle */
	DIR *dir = NULL;
	struct dirent *entry = NULL;
	

	s8_t fullName[128] = {0};

	if (*path == '\0') {
		path = "./";
	}

	s32_t len = snprintf(fullName, sizeof(fullName)-1, "%s", path);
	if (len < 0 || len >= sizeof(fullName)-1) {
		mxLogFmt(LOG_INFO,"path string '%s' too long\n",path);
		return STATUS_FAILED;
	}
	/* append a '/' if not already done */
	if (fullName[len-1] != '/') {
		fullName[len++] = '/';
		fullName[len] = '\0';
	}


	dir = opendir(path);
	if (!dir) {
		mxLogFmt(LOG_INFO,"opening directory '%s' failed: %s\n",path, strerror(errno));
		return STATUS_FAILED;
	}

	s32_t len2 = 0;
	struct stat st;
	
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}
		len2 = snprintf(fullName+len, sizeof(fullName)-len, "%s", entry->d_name);
		if (len2 < 0 || len2 >= sizeof(fullName)-len - 1) {
			mxLogFmt(LOG_INFO,"path string '%s' too long\n",path);
			closedir(dir);
			return STATUS_FAILED;
		}
		memset(&st, 0, sizeof(struct stat));
		stat(fullName, &st);
		if (!S_ISREG(st.st_mode)) {
			/* skip special file */
			continue;
		}
		loadCertByType(fullName, certType, certFlag);
	}

	closedir(dir);
	return STATUS_SUCCESS;
}

u32_t credMgr::loadCertByType(s8_t *file, u32_t certType, u32_t certFlag)
{
	switch (certType) {
		case CERT_X509:
			if (certFlag & X509_CA) {
				mpCertMgr->loadX509CaCert(file);
			}
			else if (certFlag & X509_AA) {
				mpCertMgr->loadX509AaCert(file);
			}
			else {
				mpCertMgr->loadX509Cert(file);
			}
			break;
		case CERT_X509_CRL:
			mpCertMgr->loadX509CrlCert(file);
			break;
		case CERT_X509_AC:
			mpCertMgr->loadX509AttCert(file);
			break;
		default:
			break;
	}
	return STATUS_SUCCESS;
}

