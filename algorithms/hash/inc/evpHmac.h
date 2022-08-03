#ifndef __EVP_HMAC_H__
#define __EVP_HMAC_H__
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include "hashInterface.h"


/* RFC2104 HMAC: Keyed-Hashing for Message Authentication */

class EvpHmac
{
public:
	EvpHmac();
	~EvpHmac();
	u32_t setHashType(u32_t type);
	void setHashKey(chunk_t key);
	u32_t getHmacSize(void);
	void getHashHmac(chunk_t input, chunk_t &output);

private:
	u32_t       mHashType;
	EVP_MD_CTX *mpCtx;
	const EVP_MD *mpMd;

};











#endif /* __EVP_HMAC_H__ */

