#include <unistd.h>
#include <string.h>
#include "hashSha1.h"


#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

#if BYTE_ORDER == LITTLE_ENDIAN
 #define blk0(i) (block->l[i] = (rol(block->l[i],24)&0xFF00FF00) |(rol(block->l[i],8)&0x00FF00FF))
#elif BYTE_ORDER == BIG_ENDIAN
 #define blk0(i) block->l[i]
#else
 #error "Endianness not defined!"
#endif
#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15] ^block->l[(i+2)&15]^block->l[i&15],1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);

static u8_t g_sha1padding[SHA1_BLOCK_LEN] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/**
 * RFC3174 US Secure Hash Algorithm 1 (SHA1)
 */
HashSha1::HashSha1()
{
	mState[0] = 0x67452301;
	mState[1] = 0xEFCDAB89;
	mState[2] = 0x98BADCFE;
	mState[3] = 0x10325476;
	mState[4] = 0xC3D2E1F0;
	mCount[0] = 0;
	mCount[1] = 0;
}

HashSha1::~HashSha1()
{
	mState[0] = 0;
	mState[1] = 0;
	mState[2] = 0;
	mState[3] = 0;
	mState[4] = 0;
	mCount[0] = 0;
	mCount[1] = 0;
}

u32_t HashSha1::getHashSize(void)
{
	return HASH_SIZE_SHA1;
}

u32_t HashSha1::getHashType(void)
{
	return HASH_SHA1;
}

void HashSha1::initHash(void)
{
	mState[0] = 0x67452301;
	mState[1] = 0xEFCDAB89;
	mState[2] = 0x98BADCFE;
	mState[3] = 0x10325476;
	mState[4] = 0xC3D2E1F0;
	mCount[0] = 0;
	mCount[1] = 0;
}

void HashSha1::calcHash(chunk_t chunk, u8_t *pDigist)
{
	updateSha1(chunk.ptr, chunk.len);
	if (pDigist != NULL)
	{
		finalSha1(pDigist);
		initHash();
	}
}

void HashSha1::updateSha1(u8_t *input, u32_t len)
{
	u32_t i = 0;
	u32_t index = 0;
	u32_t partLen = 0;

	/* 前一次存放数据的偏移 */
	index = (u8_t)((mCount[0] >> 3) & 0x3F);
	/* 更新 Bit 数 
	 * 如果if 条件满足，说明32位mCount[0] 溢出， 高位mCount[1] 加 1
	 */
	if ((mCount[0] += (len << 3)) < (len << 3))
	{
		mCount[1]++;
	}
	mCount[1] += (len >> 29);

	partLen = SHA1_BLOCK_LEN - index;
	/* Transform as many times as possible. */
	if (len >= partLen)
	{
		/* 拼成 64 字节 block */
		memcpy(&mBuffer[index], input, partLen);
		transformSha1(mState, mBuffer);

		for (i = partLen; i + 63 < len; i += SHA1_BLOCK_LEN)
		{
			transformSha1(mState, &input[i]);
		}
		index = 0;
	}
	else
	{
		i = 0;
	}

	/* Buffer remaining input */
	memcpy(&mBuffer[index], &input[i], len-i);

	return ;
}

void HashSha1::transformSha1(u32_t state[5], u8_t buffer[SHA1_BLOCK_LEN])
{
	u32_t a, b, c, d, e;
	typedef union {
		u8_t c[64];
		u32_t l[16];
	} CHAR64LONG16;
	CHAR64LONG16 block[1];  /* use array to appear as a pointer */
	
	memcpy(block, buffer, 64);

	/* Copy context->state[] to working vars */
	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];
	e = state[4];
	/* 4 rounds of 20 operations each. Loop unrolled. */
	R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
	R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
	R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
	R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
	R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
	R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
	R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
	R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
	R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
	R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
	R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
	R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
	R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
	R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
	R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
	R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
	R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
	R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
	R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
	R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);
	/* Add the working vars back into context.state[] */
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;
	/* Wipe variables */
	a = b = c = d = e = 0;
	memset(block, '\0', sizeof(block));
}

void HashSha1::finalSha1(u8_t digest[HASH_SIZE_SHA1])
{
	u8_t bits[8];
	u32_t index, padLen;

	/*
	 * get bit Length
	 * The two-word representation of 40 is hex 00000000 00000028.
	 * for example:
	 * 61626364 65800000 00000000 00000000
	 * 00000000 00000000 00000000 00000000
	 * 00000000 00000000 00000000 00000000
	 * 00000000 00000000 00000000 00000028
	 */
	bits[0] = (u8_t)(mCount[1] >> 24);
	bits[1] = (u8_t)(mCount[1] >> 16);
	bits[2] = (u8_t)(mCount[1] >>  8);
	bits[3] = (u8_t)(mCount[1]);
	bits[4] = (u8_t)(mCount[0] >> 24);
	bits[5] = (u8_t)(mCount[0] >> 16);
	bits[6] = (u8_t)(mCount[0] >>  8);
	bits[7] = (u8_t)(mCount[0]);
	
	/*  Append Padding Bits  
	 *  Pad out to 56 mod 64.
	 */
	index = (u8_t)((mCount[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	updateSha1 (g_sha1padding, padLen);

	/* Append Bits len */
	updateSha1(bits, 8);  

	/* get digest from mState */
	u32_t i, j;
	for (i = 0, j = 0; j < HASH_SIZE_SHA1; i++, j += 4)
	{
		digest[j]   = (u8_t)((mState[i] >> 24) & 0xff);
		digest[j+1] = (u8_t)((mState[i] >> 16) & 0xff);
		digest[j+2] = (u8_t)((mState[i] >> 8) & 0xff);
		digest[j+3] = (u8_t) (mState[i] & 0xff);
	}
}



