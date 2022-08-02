#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include "evpHash.h"


/* https://www.openssl.org/docs/manmaster/man3/EVP_DigestInit.html */
evpHash::evpHash()
{
	OpenSSL_add_all_digests();
	mpCtx = EVP_MD_CTX_new();
}

evpHash::~evpHash()
{
	if (mpCtx != NULL) {
		EVP_MD_CTX_free(mpCtx);
		mpCtx = NULL;
	}
}

u32_t evpHash::setHashType(u32_t type)
{
	switch (type) {
		case HASH_MD5:
			mpMd = EVP_get_digestbyname("md5");
			mHashType = HASH_MD5;
			break;
		case HASH_SHA1:
			mpMd = EVP_get_digestbyname("sha1");
			mHashType = HASH_SHA1;
			break;
		case HASH_SHA2_224:
			mpMd = EVP_get_digestbyname("sha224");
			mHashType = HASH_SHA2_224;
			break;

		case HASH_SHA2_256:
			mpMd = EVP_get_digestbyname("sha256");
			mHashType = HASH_SHA2_256;
			break;
		case HASH_SHA2_384:
			mpMd = EVP_get_digestbyname("sha384");
			mHashType = HASH_SHA2_384;
			break;
		case HASH_SHA2_512:
			mpMd = EVP_get_digestbyname("sha512");
			mHashType = HASH_SHA2_512;
			break;

		default:
			return 1;
	}
	return 0;
}

const char * evpHash::getHashName(void)
{
	return EVP_MD_name(mpMd);
}


u32_t evpHash::getHashType(void)
{
	return mHashType;
}

u32_t evpHash::getHashSize(void)
{
	return EVP_MD_size(mpMd);
}


void evpHash::initHash(void)
{
	EVP_DigestInit_ex(mpCtx, mpMd, NULL);
}

void evpHash::updateHash(u8_t *input, u32_t len)
{
	EVP_DigestUpdate(mpCtx, input, len);
}

void evpHash::finalHash(u8_t digest[EVP_MAX_MD_SIZE])
{
	u32_t md_len = 0;
	EVP_DigestFinal_ex(mpCtx, digest, &md_len);
}

void evpHash::calcHash(chunk_t chunk, u8_t *pDigist)
{
	unsigned char md[EVP_MAX_MD_SIZE];
	
	updateHash(chunk.ptr, chunk.len);
	if (pDigist != NULL)
	{
		finalHash(pDigist);
		initHash();
	}
}


