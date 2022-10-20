#ifndef __PRIVATE_KEY_OPENSSL_RSA_H__
#define __PRIVATE_KEY_OPENSSL_RSA_H__
#include <iostream>
#include <openssl/pem.h>
#include <openssl/pkcs12.h>
#include "mxDef.h"
#include "privateKeyRSA.h"

using namespace std;

class privateKeyOpensslRSA : public privateKeyRSA
{
public: 
	privateKeyOpensslRSA();
	~privateKeyOpensslRSA();
	
	virtual u32_t loadPriKeyFromPEM(s8_t *filename, chunk_t secret);
	virtual u32_t loadPriKeyFromDER(s8_t *filename);
	virtual u32_t calcSignature(u32_t sign_type, chunk_t data, chunk_t *signature);

private:
	static s32_t pemPasswordCb (s8_t *buf, s32_t size, s32_t rwflag, void *userdata);
	u32_t buildEmsaPkcs1Signature(const string algname, chunk_t data, chunk_t *signature);
	EVP_PKEY *mpPriKey;
};


#endif /* __PRIVATE_KEY_OPENSSL_RSA_H__ */

