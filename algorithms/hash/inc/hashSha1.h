#ifndef __HASH_SHA1_H__
#define __HASH_SHA1_H__
#include "hashInterface.h"

#define SHA1_BLOCK_LEN   64

class HashSha1 : public HashInterface
{
public:
	HashSha1();
	~HashSha1();
	virtual u32_t getHashSize(void);
	virtual u32_t getHashType(void);
	virtual void initHash(void);
	virtual void calcHash(chunk_t chunk, u8_t *pDigist);
protected:
private:
	void updateSha1(u8_t *input, u32_t len);
	void transformSha1(u32_t state[5], u8_t buffer[SHA1_BLOCK_LEN]);
	void finalSha1(u8_t digest[HASH_SIZE_SHA1]);
	u32_t mState[5];
	u32_t mCount[2];               /* 记录 bit 数 */
	u8_t  mBuffer[MD5_BLOCK_LEN];  /* 临时存放数据 */

};






#endif /* __HASH_SHA1_H__ */