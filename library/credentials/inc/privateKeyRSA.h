#ifndef __PRIVATE_KEY_RSA_H__
#define __PRIVATE_KEY_RSA_H__
#include "mxDef.h"
#include "privateKey.h"



class privateKeyRSA : public privateKey
{
public: 
	privateKeyRSA();
	~privateKeyRSA();

private:
	//RSA *mpRSA;  /* RSA object from OpenSSL */
};


#endif /* __PRIVATE_KEY_RSA_H__ */

