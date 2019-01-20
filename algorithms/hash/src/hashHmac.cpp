#include <unistd.h>
#include <string.h>
#include "hashHmac.h"
#include "hashMd5.h"
#include "hashSha1.h"
#include "hashSha2.h"

HashHmac::HashHmac()
{
	mpHash = NULL;
	mBlockLen = 0;
	memset(mIpad, 0, sizeof(mIpad));
	memset(mOpad, 0, sizeof(mOpad));
}

HashHmac::~HashHmac()
{
	if(mpHash)
	{
		delete(mpHash);
		mpHash = NULL;
	}
	mBlockLen = 0;
	memset(mIpad, 0, sizeof(mIpad));
	memset(mOpad, 0, sizeof(mOpad));
}

u32_t HashHmac::setHashType(HashType_e type)
{
	if(mpHash)
	{
		delete(mpHash);
		mpHash = NULL;
	}

	/* RFC2104   2. Definition of HMAC
	 * B(mBlockLen) is the block length of the hash function. */
	switch(type)
	{
		case HASH_MD5:
			mpHash = new HashMd5();
			mBlockLen = 64;
			break;
		case HASH_SHA1:
			mpHash = new HashSha1();
			mBlockLen = 64;
			break;
		case HASH_SHA2_224:
			mpHash = new HashSha224();
			mBlockLen = 64;
			break;
		case HASH_SHA2_256:
			mpHash = new HashSha256();
			mBlockLen = 64;
			break;
		case HASH_SHA2_384:
			mpHash = new HashSha384();
			mBlockLen = 128;
			break;
		case HASH_SHA2_512:
			mpHash = new HashSha512();
			mBlockLen = 128;
			break;
		default:
			mpHash = NULL;
			break;
	}

	return mpHash ? true:false;
}

void HashHmac::setHashKey(chunk_t key)
{
	chunk_t ipadKey = {NULL, 0};
	
	/* The authentication key K can be of any length up to B, the
	 * block length of the hash function. Applications that use keys longer
	 * than B bytes will first hash the key using H and then use the
	 * resultant L byte string as the actual key to HMAC. In any case the
	 * minimal recommended length for K is L bytes (as the hash output
	 * length).
	 */
	if(key.len > mBlockLen)
	{
		mpHash->initHash();
		mpHash->calcHash(key, mIpad);
		memcpy(mOpad, mIpad, 128);
	}
	else
	{
		memcpy(mIpad, key.ptr, key.len);
		memcpy(mOpad, key.ptr, key.len);
	}
	
	/* 
	 * We define two fixed and different strings ipad and opad as follows
	 * (the ¡¯i¡¯ and ¡¯o¡¯ are mnemonics for inner and outer):
	 * ipad = the byte 0x36 repeated B times
	 * opad = the byte 0x5C repeated B times.
	 */
	u32_t i = 0;
	for (i = 0; i < mBlockLen; i++)
	{
		mIpad[i] = mIpad[i] ^ 0x36;
		mOpad[i] = mOpad[i] ^ 0x5C;
	}
	/*  H(K XOR opad, H(K XOR ipad, text))
	 *
	 * begin hashing of inner pad
	 * H(K XOR ipad)
	 */	
	ipadKey.ptr = mIpad;
	ipadKey.len = mBlockLen;
	mpHash->initHash();
	mpHash->calcHash(ipadKey, NULL);
}

u32_t HashHmac::getHmacSize(void)
{
	u32_t size = 0;
	
	if(mpHash)
	{
		size = mpHash->getHashSize();
	}
	return size;
}

void HashHmac::getHashHmac(chunk_t input, chunk_t &output)
{
	chunk_t opadKey = {NULL, 0};
	chunk_t innerchunk = {NULL, 0};
	u8_t   buf[128]={0};

	if(output.len > mpHash->getHashSize())
	{
		return;
	}
	/* To compute HMAC over the data ¡®text¡¯ we perform
	 * H(K XOR opad, H(K XOR ipad, text)) 
	 * Namely,
	 * (1) append zeros to the end of K to create a B byte string
	 *     (e.g., if K is of length 20 bytes and B=64, then K will be
	 *     appended with 44 zero bytes 0x00)
	 * (2) XOR (bitwise exclusive-OR) the B byte string computed in step (1) with ipad
	 * (3) append the stream of data ¡¯text¡¯ to the B byte string resulting from step (2)
	 * (4) apply H to the stream generated in step (3)
	 * (5) XOR (bitwise exclusive-OR) the B byte string computed in step (1) with opad
	 * (6) append the H result from step (4) to the B byte string resulting from step (5)
	 * (7) apply H to the stream generated in step (6) and output the result
	 */
	/* H(K XOR opad, H(K XOR ipad, text))
	 *
	 * if outout is NULL, we append text to the inner hash.
	 * else, we complete the inner and do the outer.
	 */	
	if(output.ptr == NULL)
	{
		mpHash->calcHash(input, NULL);
		return;
	}
	mpHash->calcHash(input, buf);
	/* H(K XOR opad, H(K XOR ipad, text))
	 *
	 * do outer
	 */ 
	opadKey.ptr = mOpad;
	opadKey.len = mBlockLen;
	mpHash->calcHash(opadKey, NULL);
	innerchunk.ptr = buf;
	innerchunk.len = mpHash->getHashSize();
	mpHash->calcHash(innerchunk, buf);
	if(output.len == 0)
	{
		output.len = mpHash->getHashSize();
	}
	memcpy(output.ptr, buf, output.len);
	/* for next calc */
	chunk_t ipadKey = {NULL, 0};
	ipadKey.ptr = mIpad;
	ipadKey.len = mBlockLen;
	mpHash->initHash();
	mpHash->calcHash(ipadKey, NULL);
}
