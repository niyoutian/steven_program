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

static u8_t g_sha256padding[SHA256_BLOCK_LEN] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static u8_t g_sha512padding[SHA512_BLOCK_LEN] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const u64_t sha512_K[80] = {
	0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL,
	0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
	0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL,
	0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
	0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL,
	0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
	0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL, 0x2de92c6f592b0275ULL,
	0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
	0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL,
	0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
	0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL,
	0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
	0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL,
	0x92722c851482353bULL, 0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
	0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL,
	0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
	0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL,
	0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
	0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL,
	0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
	0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL,
	0xc67178f2e372532bULL, 0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
	0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL,
	0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
	0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL,
	0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
	0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
};



/**
 * RFC4634 US Secure Hash Algorithms (SHA and HMAC-SHA)
 */
HashSha224::HashSha224()
{
	mState[0] = 0xc1059ed8;
	mState[1] = 0x367cd507;
	mState[2] = 0x3070dd17;
	mState[3] = 0xf70e5939;
	mState[4] = 0xffc00b31;
	mState[5] = 0x68581511;
	mState[6] = 0x64f98fa7;
	mState[7] = 0xbefa4fa4;
	mCount[0] = 0;
	mCount[1] = 0;
}

HashSha224::~HashSha224()
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

u32_t HashSha224::getHashSize(void)
{
	return HASH_SIZE_SHA224;
}

u32_t HashSha224::getHashType(void)
{
	return HASH_SHA2_224;
}

void HashSha224::initHash(void)
{
	mState[0] = 0xc1059ed8;
	mState[1] = 0x367cd507;
	mState[2] = 0x3070dd17;
	mState[3] = 0xf70e5939;
	mState[4] = 0xffc00b31;
	mState[5] = 0x68581511;
	mState[6] = 0x64f98fa7;
	mState[7] = 0xbefa4fa4;
	mCount[0] = 0;
	mCount[1] = 0;
}

void HashSha224::calcHash(chunk_t chunk, u8_t *pDigist)
{
	updateSha224(chunk.ptr, chunk.len);
	if (pDigist != NULL)
	{
		finalSha224(pDigist);
		initHash();
	}
}

void HashSha224::updateSha224(u8_t *input, u32_t len)
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
		transformSha224(mBuffer);

		for (i = partLen; i + 63 < len; i += SHA256_BLOCK_LEN)
		{
			transformSha224(&input[i]);
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

void HashSha224::transformSha224(u8_t buffer[SHA256_BLOCK_LEN])
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
		W[t] =  SSIG1(W[t-2]) + W[t-7] + SSIG0(W[t-15]) + W[t-16];
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

void HashSha224::finalSha224(u8_t digest[HASH_SIZE_SHA224])
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
	updateSha224 (g_sha256padding, padLen);

	/* Append Bits len */
	updateSha224(bits, 8);  

	/* get digest from mState */
	u32_t i, j;
	for (i = 0, j = 0; j < HASH_SIZE_SHA224; i++, j += 4)
	{
		digest[j]   = (u8_t)((mState[i] >> 24) & 0xff);
		digest[j+1] = (u8_t)((mState[i] >> 16) & 0xff);
		digest[j+2] = (u8_t)((mState[i] >> 8) & 0xff);
		digest[j+3] = (u8_t) (mState[i] & 0xff);
	}
}

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
	return HASH_SHA2_256;
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
		finalSha256(pDigist);
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

		for (i = partLen; i + 63 < len; i += SHA256_BLOCK_LEN)
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
		W[t] =  SSIG1(W[t-2]) + W[t-7] + SSIG0(W[t-15]) + W[t-16];
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
	updateSha256 (g_sha256padding, padLen);

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


#undef CH
#undef MAJ
#undef SHR
#undef ROTR
#undef BSIG0
#undef BSIG1
#undef SSIG0
#undef SSIG1
#define CH(x,y,z)   (((x) & (y)) ^ ((~(x)) & (z)))
#define MAJ(x,y,z)  (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define SHR(n,x)    ((x) >> (n))
#define ROTR(n,x)   (((x) >> (n)) | ((x) << (64 - (n))))
#define BSIG0(x)    (ROTR(28,(x)) ^ ROTR(34,(x)) ^ ROTR(39,(x)))
#define BSIG1(x)    (ROTR(14,(x)) ^ ROTR(18,(x)) ^ ROTR(41,(x)))
#define SSIG0(x)    (ROTR(1,(x)) ^ ROTR(8,(x)) ^ SHR(7,(x)))
#define SSIG1(x)    (ROTR(19,(x)) ^ ROTR(61,(x)) ^ SHR(6,(x)))

/**
 * RFC4634 US Secure Hash Algorithms (SHA and HMAC-SHA)
 */
HashSha384::HashSha384()
{
	mState[0] = 0xcbbb9d5dc1059ed8ULL;
	mState[1] = 0x629a292a367cd507ULL;
	mState[2] = 0x9159015a3070dd17ULL;
	mState[3] = 0x152fecd8f70e5939ULL;
	mState[4] = 0x67332667ffc00b31ULL;
	mState[5] = 0x8eb44a8768581511ULL;
	mState[6] = 0xdb0c2e0d64f98fa7ULL;
	mState[7] = 0x47b5481dbefa4fa4ULL;
	mCount[0] = 0;
	mCount[1] = 0;
}

HashSha384::~HashSha384()
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

u32_t HashSha384::getHashSize(void)
{
	return HASH_SIZE_SHA384;
}

u32_t HashSha384::getHashType(void)
{
	return HASH_SHA2_384;
}

void HashSha384::initHash(void)
{
	mState[0] = 0xcbbb9d5dc1059ed8ULL;
	mState[1] = 0x629a292a367cd507ULL;
	mState[2] = 0x9159015a3070dd17ULL;
	mState[3] = 0x152fecd8f70e5939ULL;
	mState[4] = 0x67332667ffc00b31ULL;
	mState[5] = 0x8eb44a8768581511ULL;
	mState[6] = 0xdb0c2e0d64f98fa7ULL;
	mState[7] = 0x47b5481dbefa4fa4ULL;
	mCount[0] = 0;
	mCount[1] = 0;
}

void HashSha384::calcHash(chunk_t chunk, u8_t *pDigist)
{
	updateSha384(chunk.ptr, chunk.len);
	if (pDigist != NULL)
	{
		finalSha384(pDigist);
		initHash();
	}
}

void HashSha384::updateSha384(u8_t *input, u32_t len)
{
	u32_t i = 0;
	u32_t index = 0;
	u32_t partLen = 0;
	u64_t inputLen = (u64_t) len;

	/* 前一次存放数据的偏移 */
	index = (u8_t)((mCount[0] >> 3) & 0x7F);
	/* 更新 Bit 数 
	 * 如果if 条件满足，说明64位mCount[0] 溢出， 高位mCount[1] 加 1
	 */
	if ((mCount[0] += (inputLen << 3)) < (inputLen << 3))
	{
		mCount[1]++;
	}
	mCount[1] += (inputLen >> (29+32));

	partLen = SHA512_BLOCK_LEN - index;
	/* Transform as many times as possible. */
	if (len >= partLen)
	{
		/* 拼成 128 字节 block */
		memcpy(&mBuffer[index], input, partLen);
		transformSha384(mBuffer);

		for (i = partLen; i + 127 < len; i += SHA512_BLOCK_LEN)
		{
			transformSha384(&input[i]);
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

void HashSha384::transformSha384(u8_t buffer[SHA512_BLOCK_LEN])
{
	u32_t t;
	u64_t W[80];
	u8_t *pos = buffer;

	/* 1. Prepare the message schedule W:
	 * For t = 0 to 15
	 * Wt = M(i)t
	 * For t = 16 to 79
	 * Wt = SSIG1(W(t-2)) + W(t-7) + SSIG0(t-15) + W(t-16)
	 * read the data, big endian byte order
	 */
	for(t = 0; t < 16; t++)
	{
		W[t] =  (((u64_t)pos[0])<<56) | (((u64_t)pos[1])<<48) |
			    (((u64_t)pos[2])<<40) | (((u64_t)pos[3])<<32) |
			    (((u64_t)pos[4])<<24) | (((u64_t)pos[5])<<16) |
				(((u64_t)pos[6])<<8 ) | ((u64_t)pos[7]);
		pos += 8;
	}

	for(t = 16; t < 80; t++)
	{
		W[t] =  SSIG1(W[t-2]) + W[t-7] + SSIG0(W[t-15]) + W[t-16];
	}
	
	/* 2. Initialize the working variables:
	 */
	 u64_t  a, b, c, d, e, f, g, h;
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
	u64_t T1, T2;
	for(t = 0; t < 80; t++)
	{
		T1 = h + BSIG1(e) + CH(e,f,g) + sha512_K[t] + W[t];
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

void HashSha384::finalSha384(u8_t digest[HASH_SIZE_SHA384])
{
	u8_t bits[16];
	u32_t index, padLen;

	/*
	 * get bit Length 
	 * The 128-bit representation of L = 40 is hex 
	 * 00000000 00000000 00000000 00000028.
	 * for example:
	 * 61626364 65800000 00000000 00000000
	 * 00000000 00000000 00000000 00000000
	 * 00000000 00000000 00000000 00000000
	 * 00000000 00000000 00000000 00000000
	 * 00000000 00000000 00000000 00000000
	 * 00000000 00000000 00000000 00000000
	 * 00000000 00000000 00000000 00000000
	 * 00000000 00000000 00000000 00000028
	 */
	bits[0] = (u8_t)(mCount[1] >> 56);
	bits[1] = (u8_t)(mCount[1] >> 48);
	bits[2] = (u8_t)(mCount[1] >> 40);
	bits[3] = (u8_t)(mCount[1] >> 32);
	bits[4] = (u8_t)(mCount[1] >> 24);
	bits[5] = (u8_t)(mCount[1] >> 16);
	bits[6] = (u8_t)(mCount[1] >>  8);
	bits[7] = (u8_t)(mCount[1]);
	bits[8] = (u8_t)(mCount[0] >> 56);
	bits[9] = (u8_t)(mCount[0] >> 48);
	bits[10] = (u8_t)(mCount[0] >> 40);
	bits[11] = (u8_t)(mCount[0] >> 32);
	bits[12] = (u8_t)(mCount[0] >> 24);
	bits[13] = (u8_t)(mCount[0] >> 16);
	bits[14] = (u8_t)(mCount[0] >>  8);
	bits[15] = (u8_t)(mCount[0]);
	
	/*  Append Padding Bits  
	 *  Pad out to 56 mod 64.
	 */
	index = (u8_t)((mCount[0] >> 3) & 0x7F);
	padLen = (index < 112) ? (112 - index) : (240 - index);
	updateSha384 (g_sha512padding, padLen);

	/* Append Bits len */
	updateSha384(bits, 16);  

	/* get digest from mState */
	u32_t i, j;
	for (i = 0, j = 0; j < HASH_SIZE_SHA384; i++, j += 8)
	{
		digest[j]   = (u8_t)((mState[i] >> 56) & 0xff);
		digest[j+1] = (u8_t)((mState[i] >> 48) & 0xff);
		digest[j+2] = (u8_t)((mState[i] >> 40) & 0xff);
		digest[j+3] = (u8_t)((mState[i] >> 32) & 0xff);
		digest[j+4] = (u8_t)((mState[i] >> 24) & 0xff);
		digest[j+5] = (u8_t)((mState[i] >> 16) & 0xff);
		digest[j+6] = (u8_t)((mState[i] >> 8) & 0xff);
		digest[j+7] = (u8_t) (mState[i] & 0xff);
	}
}

/**
 * RFC4634 US Secure Hash Algorithms (SHA and HMAC-SHA)
 */
HashSha512::HashSha512()
{
	mState[0] = 0x6a09e667f3bcc908ULL;
	mState[1] = 0xbb67ae8584caa73bULL;
	mState[2] = 0x3c6ef372fe94f82bULL;
	mState[3] = 0xa54ff53a5f1d36f1ULL;
	mState[4] = 0x510e527fade682d1ULL;
	mState[5] = 0x9b05688c2b3e6c1fULL;
	mState[6] = 0x1f83d9abfb41bd6bULL;
	mState[7] = 0x5be0cd19137e2179ULL;
	mCount[0] = 0;
	mCount[1] = 0;
}

HashSha512::~HashSha512()
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

u32_t HashSha512::getHashSize(void)
{
	return HASH_SIZE_SHA512;
}

u32_t HashSha512::getHashType(void)
{
	return HASH_SHA2_512;
}

void HashSha512::initHash(void)
{
	mState[0] = 0x6a09e667f3bcc908ULL;
	mState[1] = 0xbb67ae8584caa73bULL;
	mState[2] = 0x3c6ef372fe94f82bULL;
	mState[3] = 0xa54ff53a5f1d36f1ULL;
	mState[4] = 0x510e527fade682d1ULL;
	mState[5] = 0x9b05688c2b3e6c1fULL;
	mState[6] = 0x1f83d9abfb41bd6bULL;
	mState[7] = 0x5be0cd19137e2179ULL;
	mCount[0] = 0;
	mCount[1] = 0;
}

void HashSha512::calcHash(chunk_t chunk, u8_t *pDigist)
{
	updateSha512(chunk.ptr, chunk.len);
	if (pDigist != NULL)
	{
		finalSha512(pDigist);
		initHash();
	}
}

void HashSha512::updateSha512(u8_t *input, u32_t len)
{
	u32_t i = 0;
	u32_t index = 0;
	u32_t partLen = 0;
	u64_t inputLen = (u64_t) len;

	/* 前一次存放数据的偏移 */
	index = (u8_t)((mCount[0] >> 3) & 0x7F);
	/* 更新 Bit 数 
	 * 如果if 条件满足，说明64位mCount[0] 溢出， 高位mCount[1] 加 1
	 */
	if ((mCount[0] += (inputLen << 3)) < (inputLen << 3))
	{
		mCount[1]++;
	}
	mCount[1] += (inputLen >> (29+32));

	partLen = SHA512_BLOCK_LEN - index;
	/* Transform as many times as possible. */
	if (len >= partLen)
	{
		/* 拼成 128 字节 block */
		memcpy(&mBuffer[index], input, partLen);
		transformSha512(mBuffer);

		for (i = partLen; i + 127 < len; i += SHA512_BLOCK_LEN)
		{
			transformSha512(&input[i]);
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

void HashSha512::transformSha512(u8_t buffer[SHA512_BLOCK_LEN])
{
	u32_t t;
	u64_t W[80];
	u8_t *pos = buffer;

	/* 1. Prepare the message schedule W:
	 * For t = 0 to 15
	 * Wt = M(i)t
	 * For t = 16 to 79
	 * Wt = SSIG1(W(t-2)) + W(t-7) + SSIG0(t-15) + W(t-16)
	 * read the data, big endian byte order
	 */
	for(t = 0; t < 16; t++)
	{
		W[t] =  (((u64_t)pos[0])<<56) | (((u64_t)pos[1])<<48) |
			    (((u64_t)pos[2])<<40) | (((u64_t)pos[3])<<32) |
			    (((u64_t)pos[4])<<24) | (((u64_t)pos[5])<<16) |
				(((u64_t)pos[6])<<8 ) | ((u64_t)pos[7]);
		pos += 8;
	}

	for(t = 16; t < 80; t++)
	{
		W[t] =  SSIG1(W[t-2]) + W[t-7] + SSIG0(W[t-15]) + W[t-16];
	}
	
	/* 2. Initialize the working variables:
	 */
	 u64_t  a, b, c, d, e, f, g, h;
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
	u64_t T1, T2;
	for(t = 0; t < 80; t++)
	{
		T1 = h + BSIG1(e) + CH(e,f,g) + sha512_K[t] + W[t];
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

void HashSha512::finalSha512(u8_t digest[HASH_SIZE_SHA512])
{
	u8_t bits[16];
	u32_t index, padLen;

	/*
	 * get bit Length 
	 * The 128-bit representation of L = 40 is hex 
	 * 00000000 00000000 00000000 00000028.
	 * for example:
	 * 61626364 65800000 00000000 00000000
	 * 00000000 00000000 00000000 00000000
	 * 00000000 00000000 00000000 00000000
	 * 00000000 00000000 00000000 00000000
	 * 00000000 00000000 00000000 00000000
	 * 00000000 00000000 00000000 00000000
	 * 00000000 00000000 00000000 00000000
	 * 00000000 00000000 00000000 00000028
	 */
	bits[0] = (u8_t)(mCount[1] >> 56);
	bits[1] = (u8_t)(mCount[1] >> 48);
	bits[2] = (u8_t)(mCount[1] >> 40);
	bits[3] = (u8_t)(mCount[1] >> 32);
	bits[4] = (u8_t)(mCount[1] >> 24);
	bits[5] = (u8_t)(mCount[1] >> 16);
	bits[6] = (u8_t)(mCount[1] >>  8);
	bits[7] = (u8_t)(mCount[1]);
	bits[8] = (u8_t)(mCount[0] >> 56);
	bits[9] = (u8_t)(mCount[0] >> 48);
	bits[10] = (u8_t)(mCount[0] >> 40);
	bits[11] = (u8_t)(mCount[0] >> 32);
	bits[12] = (u8_t)(mCount[0] >> 24);
	bits[13] = (u8_t)(mCount[0] >> 16);
	bits[14] = (u8_t)(mCount[0] >>  8);
	bits[15] = (u8_t)(mCount[0]);
	
	/*  Append Padding Bits  
	 *  Pad out to 56 mod 64.
	 */
	index = (u8_t)((mCount[0] >> 3) & 0x7F);
	padLen = (index < 112) ? (112 - index) : (240 - index);
	updateSha512 (g_sha512padding, padLen);

	/* Append Bits len */
	updateSha512(bits, 16);  

	/* get digest from mState */
	u32_t i, j;
	for (i = 0, j = 0; j < HASH_SIZE_SHA512; i++, j += 8)
	{
		digest[j]   = (u8_t)((mState[i] >> 56) & 0xff);
		digest[j+1] = (u8_t)((mState[i] >> 48) & 0xff);
		digest[j+2] = (u8_t)((mState[i] >> 40) & 0xff);
		digest[j+3] = (u8_t)((mState[i] >> 32) & 0xff);
		digest[j+4] = (u8_t)((mState[i] >> 24) & 0xff);
		digest[j+5] = (u8_t)((mState[i] >> 16) & 0xff);
		digest[j+6] = (u8_t)((mState[i] >> 8) & 0xff);
		digest[j+7] = (u8_t) (mState[i] & 0xff);
	}
}
