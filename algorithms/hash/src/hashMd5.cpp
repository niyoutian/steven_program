#include <unistd.h>
#include <string.h>
#include "hashMd5.h"

/* Constants for MD5Transform routine. */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

static u8_t g_md5padding[MD5_BLOCK_LEN] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* RFC1321 3.4 Step 4. Process Message in 16-Word Blocks
 * F(X,Y,Z) = XY v not(X) Z
 * G(X,Y,Z) = XZ v Y not(Z)
 * H(X,Y,Z) = X xor Y xor Z
 * I(X,Y,Z) = Y xor (X v not(Z))
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
 */
/* a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s) */
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (u32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
/* a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s) */
#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (u32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
/* a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s) */
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (u32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
/* a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s) */
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (u32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

/** 
 * �ֽ���  https://www.aliyun.com/jiaocheng/191395.html
 */
#if BYTE_ORDER != LITTLE_ENDIAN

/* Encodes input (u32_t) into output (u8_t). Assumes len is
 * a multiple of 4.
 */
static void Encode (u8_t *output, u32_t *input, u32_t len)
{
	u32_t i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
	{
		output[j] = (u8_t)(input[i] & 0xff);
		output[j+1] = (u8_t)((input[i] >> 8) & 0xff);
		output[j+2] = (u8_t)((input[i] >> 16) & 0xff);
		output[j+3] = (u8_t)((input[i] >> 24) & 0xff);
	}
}

/* Decodes input (u8_t) into output (u32_t). Assumes len is
 * a multiple of 4.
 */
static void Decode(u32_t *output, u8_t *input, u32_t len)
{
	u32_t i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
	{
		output[i] = ((u32_t)input[j]) | (((u32_t)input[j+1]) << 8) |
		(((u32_t)input[j+2]) << 16) | (((u32_t)input[j+3]) << 24);
	}
}

#elif BYTE_ORDER == LITTLE_ENDIAN
 #define Encode memcpy
 #define Decode memcpy
#endif

/**
 * RFC1321 The MD5 Message-Digest Algorithm
 */
HashMd5::HashMd5()
{
	mState[0] = 0x67452301;
	mState[1] = 0xefcdab89;
	mState[2] = 0x98badcfe;
	mState[3] = 0x10325476;
	mCount[0] = 0;
	mCount[1] = 0;
}

HashMd5::~HashMd5()
{
	mState[0] = 0;
	mState[1] = 0;
	mState[2] = 0;
	mState[3] = 0;
	mCount[0] = 0;
	mCount[1] = 0;
}

u32_t HashMd5::getHashSize(void)
{
	return HASH_SIZE_MD5;
}

u32_t HashMd5::getHashType(void)
{
	return HASH_MD5;
}

void HashMd5::initHash(void)
{
	mState[0] = 0x67452301;
	mState[1] = 0xefcdab89;
	mState[2] = 0x98badcfe;
	mState[3] = 0x10325476;
	mCount[0] = 0;
	mCount[1] = 0;
}

void HashMd5::calcHash(chunk_t chunk, u8_t *pDigist)
{
	updateMd5(chunk.ptr, chunk.len);
	if (pDigist != NULL)
	{
		finalMd5(pDigist);
		initHash();
	}
}

void HashMd5::updateMd5(u8_t *input, u32_t len)
{
	u32_t i = 0;
	u32_t index = 0;
	u32_t partLen = 0;

	/* ǰһ�δ�����ݵ�ƫ�� */
	index = (u8_t)((mCount[0] >> 3) & 0x3F);
	/* ���� Bit �� 
	 * ���if �������㣬˵��32λmCount[0] ����� ��λmCount[1] �� 1
	 */
	if ((mCount[0] += (len << 3)) < (len << 3))
	{
		mCount[1]++;
	}
	mCount[1] += (len >> 29);

	partLen = MD5_BLOCK_LEN - index;
	/* Transform as many times as possible. */
	if (len >= partLen)
	{
		/* ƴ�� 64 �ֽ� block */
		memcpy(&mBuffer[index], input, partLen);
		transformMd5(mState, mBuffer);

		for (i = partLen; i + 63 < len; i += MD5_BLOCK_LEN)
		{
			transformMd5(mState, &input[i]);
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

/** ��� Step 1 (Append Padding Bits) �� Step 2 (Append Length)
 *  ������ϢӦ����512 bit (16 (32-bit) words) �������� 
 *  Let M[0 ... N-1] denote the words of the resulting message,
 *  where N is a multiple of 16.
 *
 *  Do the following:
 *  Process each 16-word block.
 *  For i = 0 to N/16-1 do  (ÿ�δ���512 bit)
 *  Copy block i into X.
 *  For j = 0 to 15 do
 *      Set X[j] to M[i*16+j].
 *  end
 */
void HashMd5::transformMd5(u32_t state[4], u8_t block[MD5_BLOCK_LEN])
{
	u32_t a = state[0], b = state[1], c = state[2], d = state[3], x[16];

	Decode(x, block, MD5_BLOCK_LEN);

	/* Round 1 */
	/* 
	 * a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s) 
	 * This step uses a 64-element table T[1 ... 64] constructed from the
	 * sine function. Let T[i] denote the i-th element of the table, which
	 * is equal to the integer part of 4294967296 times abs(sin(i)), where i
	 * is in radians(����).
	 * 360��(�Ƕ�) = 2��(����)
	 * sin(1����) = sin(360/2��)   
	 * sin(360/2��) * 4294967296  = 0xd76aa478
	 */
	
	FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
	FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
	FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
	FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
	FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
	FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
	FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
	FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
	FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
	FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
	FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	/* Round 2 */
	GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
	GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
	GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
	GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
	GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
	GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
	GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
	GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
	GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
	GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
	GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
	GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
	HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
	HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
	HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
	HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
	HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
	HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
	HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
	HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
	HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
	HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

	/* Round 4 */
	II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
	II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
	II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
	II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
	II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
	II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
	II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
	II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
	II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
	II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
}

void HashMd5::finalMd5(u8_t digest[16])
{
	u8_t bits[8];
	u32_t index, padLen;

	/* Save number of bits */
	Encode (bits, mCount, 8);

	/* Pad out to 56 mod 64. */
	index = (u8_t)((mCount[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	updateMd5 (g_md5padding, padLen);

	/* Append length (before padding) */
	updateMd5 (bits, 8);

	Encode (digest, mState, 16);
}
