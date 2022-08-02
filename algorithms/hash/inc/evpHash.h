#ifndef __EVP_HASH_H__
#define __EVP_HASH_H__
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include "hashInterface.h"


class evpHash : public HashInterface
{
public:
	evpHash();
	~evpHash();
	virtual u32_t getHashSize(void);
	virtual u32_t getHashType(void);
	u32_t setHashType(u32_t type);
	const char* getHashName(void);
	virtual void initHash(void);
	virtual void calcHash(chunk_t chunk, u8_t *pDigist);
protected:
private:
	void updateHash(u8_t *input, u32_t len);
	void finalHash(u8_t  digest[EVP_MAX_MD_SIZE]);
	u32_t       mHashType;
	EVP_MD_CTX *mpCtx;
	const EVP_MD *mpMd;

};






#endif /* __EVP_HASH_H__ */

