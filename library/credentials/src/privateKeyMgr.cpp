#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "mxLog.h"
#include "privateKeyMgr.h"
#include "privateKeyGmpRSA.h"
#include "asn1Parser.h"





privateKeyMgr::privateKeyMgr()
{
	
}


privateKeyMgr::~privateKeyMgr()
{
	
}

privateKey* privateKeyMgr::loadPkcs1RsaPrivateKey(s8_t *filename,chunk_t secret)
{
	u32_t status = 0;
	u32_t encoding = 0;
	
	status = getKeyEncoding(filename, encoding);
	if (status != STATUS_SUCCESS) {
		return NULL;
	}

	privateKey *pRsaKey = new privateKeyGmpRSA();
	
	if (encoding == KEY_ENCODING_PEM) {
		pRsaKey->loadPriKeyFromPEM(filename, secret);
	} else if (encoding == KEY_ENCODING_DER) {
		//pRsaKey->loadX509CertFromDER(filename);
	} else {
		delete pRsaKey;
		return NULL;
	}

	mPriKeyList.push_back(pRsaKey);

	/*chunk_t chunk = pCert->getCertSubjectString();
	mxLogFmt(LOG_DEBUG,"loaded CA certificate %s from %s\n",chunk.ptr, filename);
	chunk_free(&chunk);*/
	
	return pRsaKey;
}

/*
.DER .CER，文件是二进制格式，只保存证书，不保存私钥。
.PEM，一般是文本格式，可保存证书，可保存私钥。

*/
u32_t privateKeyMgr::getKeyEncoding(s8_t *filename,u32_t& encoding)
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
		encoding = KEY_ENCODING_PEM;
		ret = STATUS_SUCCESS;
	} else {
		chunk_t blob =  {(u8_t *)buf, sb.st_size};
		asn1Parser *parser = new asn1Parser(NULL, {NULL, 0});
		if (parser->isAsn1Encode(blob)) {
			encoding = KEY_ENCODING_DER;
			ret = STATUS_SUCCESS;
		} else {
			ret = STATUS_FAILED;
		}
		delete parser;
	}

	close(fd);
	free(buf);
	return ret;
}


