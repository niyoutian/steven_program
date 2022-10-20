#ifndef __PRIVATE_KEY_RSA_H__
#define __PRIVATE_KEY_RSA_H__
#include "mxDef.h"
#include "privateKey.h"



class privateKeyRSA : public privateKey
{
public: 
	privateKeyRSA();
	~privateKeyRSA();
	virtual u32_t getKeyType(void);
	virtual u32_t loadPriKeyFromPEM(s8_t *filename, chunk_t secret) = 0;
	virtual u32_t loadPriKeyFromDER(s8_t *filename) = 0;
	virtual u32_t calcSignature(u32_t sign_type, chunk_t data, chunk_t *signature) = 0;
private:
	
};


#endif /* __PRIVATE_KEY_RSA_H__ */

