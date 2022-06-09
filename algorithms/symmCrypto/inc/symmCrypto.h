#ifndef __SYMM_CRYPTO_H__
#define __SYMM_CRYPTO_H__
#include "mxDef.h"
#include "symmCryptoBase.h"


class mxSymmEnc
{
public: 
	mxSymmEnc();
	~mxSymmEnc();
	virtual u32_t setKey(void) = 0;
	virtual u32_t getKeySize(void) = 0;
	virtual u32_t getIvSize(void) = 0;
	virtual u32_t getBlockSize(void) = 0;

private:
	mxSymmEncBase *mpEnc;

};


#endif /* __SYMM_CRYPTO_H__ */

