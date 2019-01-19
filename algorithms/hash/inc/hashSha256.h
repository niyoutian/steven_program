#ifndef __HASH_SHA256_H__
#define __HASH_SHA256_H__
#include "hashInterface.h"

#define SHA256_BLOCK_LEN   64
#define SHA512_BLOCK_LEN   128

class HashSha224 : public HashInterface
{
public:
	HashSha224();
	~HashSha224();
	virtual u32_t getHashSize(void);
	virtual u32_t getHashType(void);
	virtual void initHash(void);
	virtual void calcHash(chunk_t chunk, u8_t *pDigist);
protected:
private:
	void updateSha224(u8_t *input, u32_t len);
	void transformSha224(u8_t block[SHA256_BLOCK_LEN]);
	void finalSha224(u8_t digest[HASH_SIZE_SHA224]);
	u32_t mState[8];
	u32_t mCount[2];               /* 记录 bit 数 */
	u8_t  mBuffer[SHA256_BLOCK_LEN];  /* 临时存放数据 */

};

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

class HashSha384 : public HashInterface
{
public:
	HashSha384();
	~HashSha384();
	virtual u32_t getHashSize(void);
	virtual u32_t getHashType(void);
	virtual void initHash(void);
	virtual void calcHash(chunk_t chunk, u8_t *pDigist);
protected:
private:
	void updateSha384(u8_t *input, u32_t len);
	void transformSha384(u8_t block[SHA512_BLOCK_LEN]);
	void finalSha384(u8_t digest[HASH_SIZE_SHA384]);
	u64_t mState[8];
	u64_t mCount[2];               /* 记录 bit 数 */
	u8_t  mBuffer[SHA512_BLOCK_LEN];  /* 临时存放数据 */

};

class HashSha512 : public HashInterface
{
public:
	HashSha512();
	~HashSha512();
	virtual u32_t getHashSize(void);
	virtual u32_t getHashType(void);
	virtual void initHash(void);
	virtual void calcHash(chunk_t chunk, u8_t *pDigist);
protected:
private:
	void updateSha512(u8_t *input, u32_t len);
	void transformSha512(u8_t block[SHA512_BLOCK_LEN]);
	void finalSha512(u8_t digest[HASH_SIZE_SHA512]);
	u64_t mState[8];
	u64_t mCount[2];               /* 记录 bit 数 */
	u8_t  mBuffer[SHA512_BLOCK_LEN];  /* 临时存放数据 */

};




#endif /* __HASH_SHA256_H__ */

