#include "symmCryptoSsl.h"


SymmCryptoSSl::SymmCryptoSSl()
{
	mEncryptionType = 0;
	mpCipher = NULL;
	mpKey = NULL;
	mKeyLen = 0;
}

SymmCryptoSSl::~SymmCryptoSSl()
{
	mEncryptionType = 0;
	mpCipher = NULL;
	if (mpKey != NULL) {
		free(mpKey);
	}
	mpKey = NULL;
	mKeyLen = 0;
}

u32_t SymmCryptoSSl::setCryptoType(u32_t encType, u32_t keySize)
{
	mEncryptionType = encType;

	switch (encType) {
		case ENCR_NULL:
			mpCipher = EVP_enc_null();
			mKeyLen = 0;
			break;
		case ENCR_AES_CBC:
			switch (keySize) {
				case 16:		/* AES-128 */
					mpCipher = EVP_get_cipherbyname("aes-128-cbc");
					break;
				case 24:		/* AES-192 */
					mpCipher = EVP_get_cipherbyname("aes-192-cbc");
					break;
				case 32:		/* AES-256 */
					mpCipher = EVP_get_cipherbyname("aes-256-cbc");
					break;
				default:
					break;
			}
			mKeyLen = keySize;
			break;
		case ENCR_CAMELLIA_CBC:
			switch (keySize) {
				case 16:		/* CAMELLIA 128 */
					mpCipher = EVP_get_cipherbyname("camellia-128-cbc");
					break;
				case 24:		/* CAMELLIA 192 */
					mpCipher = EVP_get_cipherbyname("camellia-192-cbc");
					break;
				case 32:		/* CAMELLIA 256 */
					mpCipher = EVP_get_cipherbyname("camellia-256-cbc");
					break;
				default:
					break;
			}
			mKeyLen = keySize;
			break;
		case ENCR_DES_ECB:
			mKeyLen = 8;
			mpCipher = EVP_des_ecb();
			break;
		default:
			/* OpenSSL does not support the requested algo */
			break;

	}
	if (mpCipher == NULL) {
		return 1;
	}

	mpKey = (u8_t*)malloc(mKeyLen+1);
	return 0;
}

u32_t SymmCryptoSSl::setKey(u8_t *pKey, u32_t keyLen)
{
	memcpy(mpKey, pKey, mKeyLen);
	return 0;
}


u32_t SymmCryptoSSl::getKeySize(void)
{
	return mKeyLen;
}


u32_t SymmCryptoSSl::getBlockSize(void)
{
	return EVP_CIPHER_block_size(mpCipher);
}

u32_t SymmCryptoSSl::getIvSize(void)
{
	return EVP_CIPHER_iv_length(mpCipher);
}

u32_t SymmCryptoSSl::crypto(chunk_t data, chunk_t iv, chunk_t *dst, u32_t dir)
{
	EVP_CIPHER_CTX *pCtx = NULL;
	s32_t len = 0;

	pCtx = EVP_CIPHER_CTX_new();
	/**
	int EVP_CipherInit_ex(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *type,
                       ENGINE *impl, const unsigned char *key, const unsigned char *iv, int enc);
	函数作用: 初始化密码上下文ctx
	ctx  : 由 EVP_CIPHER_CTX_new() 创建
	type : 使用的算法类型例如EVP_aes_256_cbc()、EVP_aes_128_cbc()
	impl 密码类型;如果impl为 NULL;则使用默认实现。一般都设置为NULL
	key  : 加密密钥
	iv	 : 偏移量
	dir  : 1 - 加密;0 - 解密
	**/	

	if (pCtx == NULL) {
		return -1;
	}
	EVP_CipherInit_ex(pCtx, mpCipher, NULL, mpKey, iv.ptr, dir);

	
	/**
	int EVP_CIPHER_CTX_set_padding(EVP_CIPHER_CTX *x, int padding);
	启用或禁用填充。在使用 EVP_EncryptInit_ex()、EVP_DecryptInit_ex() 或 EVP_CipherInit_ex() 为加密或解密设置上下文后应调用此函数。
	默认情况下加密操作使用标准块填充进行填充并且在解密时检查并删除填充。
	如果填充参数 padding 设置为零则不执行填充;此时加密或解密的数据总量必须是块大小的倍数;否则将发生错误。
	
	默认情况下填充是启用的。padding 为0 则禁用填充;否则启用填充
	**/
	EVP_CIPHER_CTX_set_padding(pCtx, 0); /* disable padding */

	/**
	int EVP_CipherUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out,
                      int *outl, const unsigned char *in, int inl);
	输入 in 缓冲区中的 inl 字节的数据并将加/解密数据写入 out。可以多次调用此函数来加/解密连续的数据块。
	**/
	EVP_CipherUpdate(pCtx, dst->ptr, &len, data.ptr, data.len);

	/**
	int EVP_CipherFinal_ex(EVP_CIPHER_CTX *ctx, unsigned char *outm, int *outl);
	输出 缓冲区中剩余的数据。必须在 EVP_CipherUpdate() 之后调用。
	outm : 为输出缓冲区中剩余部分
	**/
	EVP_CipherFinal_ex(pCtx, dst->ptr + len, &len);

	
	EVP_CIPHER_CTX_free(pCtx);
	
	return 0;
}


u32_t SymmCryptoSSl::encrypt(chunk_t data, chunk_t iv, chunk_t *dst)
{
	return crypto(data, iv, dst, 1);
}

u32_t SymmCryptoSSl::decrypt(chunk_t data, chunk_t iv, chunk_t *dst)
{
	return crypto(data, iv, dst, 0);
}


