#ifndef __HASH_SHA256_H__
#define __HASH_SHA256_H__
#include "hashInterface.h"

#define SHA256_BLOCK_LEN   64

class HashSha256 : public HashInterface
{
public:
	HashSha256();
	~HashSha256();
	virtual u32_t getHashSize(void);
	virtual u32_t getHashType(void);
	virtual void initHash(void);
	virtual void calcHash(chunk_t chunk, u8_t *pDigist);
protected:
private:
	void updateSha256(u8_t *input, u32_t len);
	void transformSha256(u8_t block[SHA256_BLOCK_LEN]);
	void finalSha256(u8_t digest[HASH_SIZE_SHA256]);
	u32_t mState[8];
	u32_t mCount[2];               /* 记录 bit 数 */
	u8_t  mBuffer[SHA256_BLOCK_LEN];  /* 临时存放数据 */

};






#endif /* __HASH_SHA256_H__ */

