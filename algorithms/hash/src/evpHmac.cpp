#include <unistd.h>
#include <string.h>
#include "evpHmac.h"



EvpHmac::EvpHmac()
{
	OpenSSL_add_all_digests();
	mpCtx = EVP_MD_CTX_new();
}

EvpHmac::~EvpHmac()
{
	if (mpCtx != NULL) {
		EVP_MD_CTX_free(mpCtx);
		mpCtx = NULL;
	}

}

u32_t EvpHmac::setHashType(u32_t type)
{
	switch (type) {
		case HASH_MD4:
			mpMd = EVP_get_digestbyname("MD4");
			mHashType = HASH_MD4;
			break;

		case HASH_MD5:
			mpMd = EVP_get_digestbyname("MD5");
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
			mpMd = EVP_get_digestbyname("SHA512");
			mHashType = HASH_SHA2_512;
			break;
		case HASH_SHA3_224:
			mpMd = EVP_get_digestbyname("SHA3-224");
			mHashType = HASH_SHA3_224;
			break;
		case HASH_SHA3_256:
			mpMd = EVP_get_digestbyname("SHA3-256");
			mHashType = HASH_SHA3_256;
			break;
		case HASH_SHA3_384:
			mpMd = EVP_get_digestbyname("SHA3-384");
			mHashType = HASH_SHA3_384;
			break;
		case HASH_SHA3_512:
			mpMd = EVP_get_digestbyname("SHA3-512");
			mHashType = HASH_SHA3_512;
			break;

		case HASH_SM3:
			mpMd = EVP_get_digestbyname("SM3");
			mHashType = HASH_SM3;
			break;

		default:
			return 1;
	}
	return 0;
}

void EvpHmac::setHashKey(chunk_t key)
{
	EVP_PKEY *pkey = NULL;
	
	pkey = EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL, key.ptr, key.len);

	EVP_DigestSignInit(mpCtx, NULL, mpMd, NULL, pkey);
	
}

u32_t EvpHmac::getHmacSize(void)
{
	return EVP_MD_size(mpMd);
}

void EvpHmac::getHashHmac(chunk_t input, chunk_t &output)
{
	u8_t md_value[EVP_MAX_MD_SIZE];   
	size_t md_len = 0;

	if(output.len > getHmacSize()){
		return;
	}
	
	if(output.ptr == NULL){
		EVP_DigestSignUpdate(mpCtx, input.ptr, input.len);
		return ;
	}
	
	EVP_DigestSignUpdate(mpCtx, input.ptr, input.len);
	EVP_DigestSignFinal(mpCtx, md_value, &md_len);

	if(output.len == 0)
	{
		output.len = getHmacSize();
	}

	memcpy(output.ptr, md_value, output.len);

	return;
}

