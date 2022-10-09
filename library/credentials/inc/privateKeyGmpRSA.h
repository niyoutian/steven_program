#ifndef __PRIVATE_KEY_GMP_RSA_H__
#define __PRIVATE_KEY_GMP_RSA_H__
#include "mxDef.h"
#include "privateKeyRSA.h"



class privateKeyGmpRSA : public privateKeyRSA
{
public: 
	privateKeyGmpRSA();
	~privateKeyGmpRSA();
	virtual u32_t getKeyType(void) = 0;
private:
	//RSA *mpRSA;  /* RSA object from OpenSSL */
};


#endif /* __PRIVATE_KEY_GMP_RSA_H__ */

