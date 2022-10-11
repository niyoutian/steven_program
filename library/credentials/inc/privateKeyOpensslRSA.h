#ifndef __PRIVATE_KEY_OPENSSL_RSA_H__
#define __PRIVATE_KEY_OPENSSL_RSA_H__
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
	//RSA *mpRSA;  /* RSA object from OpenSSL */
};


#endif /* __PRIVATE_KEY_OPENSSL_RSA_H__ */

