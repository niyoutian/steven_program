#ifndef __SYMM_CRYPTO_SSL_H__
#define __SYMM_CRYPTO_SSL_H__
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include "symmCryptoBase.h"



class SymmCryptoSSl : public mxSymmEncBase
{
public: 
	SymmCryptoSSl();
	~SymmCryptoSSl();
	u32_t setCryptoType(u32_t encType, u32_t keySize);
	u32_t setKey(u8_t *pKey, u32_t keyLen);
	u32_t getKeySize(void);
	u32_t getIvSize(void);
	u32_t getBlockSize(void);
	u32_t encrypt(chunk_t data, chunk_t iv, chunk_t *dst);
	u32_t decrypt(chunk_t data, chunk_t iv, chunk_t *dst);

	
private:
	u32_t crypto(chunk_t data, chunk_t iv, chunk_t *dst, u32_t dir);
	
	u32_t             mEncryptionType;
	const EVP_CIPHER *mpCipher;
	u8_t             *mpKey;
	u32_t		      mKeyLen;

};


#endif /* __SYMM_CRYPTO_SSL_H__ */
