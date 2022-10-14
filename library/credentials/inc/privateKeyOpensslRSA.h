#ifndef __PRIVATE_KEY_OPENSSL_RSA_H__
#define __PRIVATE_KEY_OPENSSL_RSA_H__
#include <openssl/pem.h>
#include <openssl/pkcs12.h>
#include "mxDef.h"
#include "privateKeyRSA.h"



class privateKeyOpensslRSA : public privateKeyRSA
{
public: 
	privateKeyOpensslRSA();
	~privateKeyOpensslRSA();
	
	virtual u32_t loadPriKeyFromPEM(s8_t *filename, chunk_t secret);
	virtual u32_t loadPriKeyFromDER(s8_t *filename);

private:
	static s32_t pemPasswordCb (s8_t *buf, s32_t size, s32_t rwflag, void *userdata);
	
	EVP_PKEY *mpPriKey;
};


#endif /* __PRIVATE_KEY_OPENSSL_RSA_H__ */

