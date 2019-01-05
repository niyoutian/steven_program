#ifndef __HASH_MD5_H__
#define __HASH_MD5_H__
#include "hashInterface.h"

#define MD5_BLOCK_LEN   64

class HashMd5 : public HashInterface
{
public:
	HashMd5();
	~HashMd5();
	virtual u32_t getHashSize(void);
	virtual u32_t getHashType(void);
	virtual void initHash(void);
	virtual void calcHash(chunk_t chunk, u8_t *pDigist);
protected:
private:
	void updateMd5(u8_t *input, u32_t len);
	void transformMd5(u32_t state[4], u8_t block[MD5_BLOCK_LEN]);
	void finalMd5(u8_t digest[16]);
	u32_t mState[4];
	u32_t mCount[2];               /* 记录 bit 数 */
	u8_t  mBuffer[MD5_BLOCK_LEN];  /* 临时存放数据 */

};






#endif /* __HASH_MD5_H__ */