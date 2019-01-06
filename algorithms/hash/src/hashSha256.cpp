#include <unistd.h>
#include <string.h>
#include "hashSha256.h"

/*  CH( x, y, z) = (x AND y) XOR ( (NOT x) AND z)
 *  MAJ( x, y, z) = (x AND y) XOR (x AND z) XOR (y AND z)
 *  BSIG0(x) = ROTR^2(x) XOR ROTR^13(x) XOR ROTR^22(x)
 *  BSIG1(x) = ROTR^6(x) XOR ROTR^11(x) XOR ROTR^25(x)
 *  SSIG0(x) = ROTR^7(x) XOR ROTR^18(x) XOR SHR^3(x)
 *  SSIG1(x) = ROTR^17(x) XOR ROTR^19(x) XOR SHR^10(x)
 */
#define CH(x,y,z)   (((x) & (y)) ^ ((~(x)) & (z)))
#define MAJ(x,y,z)  (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define SHR(n,x)    ((x) >> (n))
#define ROTR(n,x)   (((x) >> (n)) | ((x) << (32 - (n))))
#define BSIG0(x)    (ROTR(2,(x)) ^ ROTR(13,(x)) ^ ROTR(22,(x)))
#define BSIG1(x)    (ROTR(6,(x)) ^ ROTR(11,(x)) ^ ROTR(25,(x)))
#define SSIG0(x)    (ROTR(7,(x)) ^ ROTR(18,(x)) ^ SHR(3,(x)))
#define SSIG1(x)    (ROTR(17,(x)) ^ ROTR(19,(x)) ^ SHR(10,(x)))

static const u32_t sha256_K[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
	0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
	0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
	0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
	0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
	0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};


/**
 * RFC4634 US Secure Hash Algorithms (SHA and HMAC-SHA)
 */
HashSha256::HashSha256()
{
	mState[0] = 0x6a09e667;
	mState[1] = 0xbb67ae85;
	mState[2] = 0x3c6ef372;
	mState[3] = 0xa54ff53a;
	mState[4] = 0x510e527f;
	mState[5] = 0x9b05688c;
	mState[6] = 0x1f83d9ab;
	mState[7] = 0x5be0cd19;
	mCount[0] = 0;
	mCount[1] = 0;
}

HashSha256::~HashSha256()
{
	mState[0] = 0;
	mState[1] = 0;
	mState[2] = 0;
	mState[3] = 0;
	mState[4] = 0;
	mState[5] = 0;
	mState[6] = 0;
	mState[7] = 0;
	mCount[0] = 0;
	mCount[1] = 0;
}

u32_t HashSha256::getHashSize(void)
{
	return HASH_SIZE_SHA256;
}

u32_t HashSha256::getHashType(void)
{
	return HASH_SHA256;
}

void HashSha256::initHash(void)
{
	mState[0] = 0x6a09e667;
	mState[1] = 0xbb67ae85;
	mState[2] = 0x3c6ef372;
	mState[3] = 0xa54ff53a;
	mState[4] = 0x510e527f;
	mState[5] = 0x9b05688c;
	mState[6] = 0x1f83d9ab;
	mState[7] = 0x5be0cd19;
	mCount[0] = 0;
	mCount[1] = 0;
}

void HashSha256::calcHash(chunk_t chunk, u8_t *pDigist)
{
	updateSha256(chunk.ptr, chunk.len);
	if (pDigist != NULL)
	{
		//finalSha1(pDigist);
		initHash();
	}
}

void HashSha256::updateSha256(u8_t *input, u32_t len)
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

	partLen = SHA256_BLOCK_LEN - index;
	/* Transform as many times as possible. */
	if (len >= partLen)
	{
		/* 拼成 64 字节 block */
		memcpy(&mBuffer[index], input, partLen);
		transformSha256(mBuffer);

		for (i = partLen; i + 63 < len; i += SHA1_BLOCK_LEN)
		{
			transformSha256(&input[i]);
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

void HashSha256::transformSha256(u8_t buffer[SHA256_BLOCK_LEN])
{
	u32_t t;
	u32_t W[SHA256_BLOCK_LEN];
	u8_t *pos = buffer;

	/* 1. Prepare the message schedule W:
	 * For t = 0 to 15
	 * Wt = M(i)t
	 * For t = 16 to 63
	 * Wt = SSIG1(W(t-2)) + W(t-7) + SSIG0(t-15) + W(t-16)
	 * read the data, big endian byte order
	 */
	for(t = 0; t < 16; t++)
	{
		W[t] =  (((u32_t)pos[0])<<24) | (((u32_t)pos[1])<<16) |
				(((u32_t)pos[2])<<8 ) | ((u32_t)pos[3]);
		pos += 4;
	}

	for(t = 16; t < 64; t++)
	{
		W[t] =  SSIG1(W[t-2]) + W[t-7] + SSIG0(t-15) + W[t-16];
	}
	
	/* 2. Initialize the working variables:
	 */
	 u32_t  a, b, c, d, e, f, g, h;
	a = mState[0];
	b = mState[1];
	c = mState[2];
	d = mState[3];
	e = mState[4];
	f = mState[5];
	g = mState[6];
	h = mState[7];
	
	/* 3. Perform the main hash computation:
	 * For t = 0 to 63
	 * T1 = h + BSIG1(e) + CH(e,f,g) + Kt + Wt
	 * T2 = BSIG0(a) + MAJ(a,b,c)
	 * h = g
	 * g = f
	 * f = e
	 * e = d + T1
	 * d = c
	 * c = b
	 * b = a
	 * a = T1 + T2
	 */
	u32_t T1, T2;
	for(t = 0; t < 64; t++)
	{
		T1 = h + BSIG1(e) + CH(e,f,g) + sha256_K[t] + W[t];
		T2 = BSIG0(a) + MAJ(a,b,c);
		h = g;
		g = f;
		f = e;
		e = d + T1;
		d = c;
		c = b;
		b = a;
		a = T1 + T2;
	}

	/* 4. Compute the intermediate hash value H(i):
	 * H(i)0 = a + H(i-1)0
	 * H(i)1 = b + H(i-1)1
	 * H(i)2 = c + H(i-1)2
	 * H(i)3 = d + H(i-1)3
	 * H(i)4 = e + H(i-1)4
	 * H(i)5 = f + H(i-1)5
	 * H(i)6 = g + H(i-1)6
	 * H(i)7 = h + H(i-1)7
	 */
	mState[0] += a;
	mState[1] += b;
	mState[2] += c;
	mState[3] += d;
	mState[4] += e;
	mState[5] += f;
	mState[6] += g;
	mState[7] += h;
}

void HashSha256::finalSha256(u8_t digest[HASH_SIZE_SHA256])
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
	updateSha256 (g_sha1padding, padLen);

	/* Append Bits len */
	updateSha256(bits, 8);  

	/* get digest from mState */
	u32_t i, j;
	for (i = 0, j = 0; j < HASH_SIZE_SHA256; i++, j += 4)
	{
		digest[j]   = (u8_t)((mState[i] >> 24) & 0xff);
		digest[j+1] = (u8_t)((mState[i] >> 16) & 0xff);
		digest[j+2] = (u8_t)((mState[i] >> 8) & 0xff);
		digest[j+3] = (u8_t) (mState[i] & 0xff);
	}
}


