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
	��������: ��ʼ������������ctx
	ctx  : �� EVP_CIPHER_CTX_new() ����
	type : ʹ�õ��㷨��������EVP_aes_256_cbc()��EVP_aes_128_cbc()
	impl ��������;���implΪ NULL;��ʹ��Ĭ��ʵ�֡�һ�㶼����ΪNULL
	key  : ������Կ
	iv	 : ƫ����
	dir  : 1 - ����;0 - ����
	**/	

	if (pCtx == NULL) {
		return -1;
	}
	EVP_CipherInit_ex(pCtx, mpCipher, NULL, mpKey, iv.ptr, dir);

	
	/**
	int EVP_CIPHER_CTX_set_padding(EVP_CIPHER_CTX *x, int padding);
	���û������䡣��ʹ�� EVP_EncryptInit_ex()��EVP_DecryptInit_ex() �� EVP_CipherInit_ex() Ϊ���ܻ�������������ĺ�Ӧ���ô˺�����
	Ĭ������¼��ܲ���ʹ�ñ�׼����������䲢���ڽ���ʱ��鲢ɾ����䡣
	��������� padding ����Ϊ����ִ�����;��ʱ���ܻ���ܵ��������������ǿ��С�ı���;���򽫷�������
	
	Ĭ���������������õġ�padding Ϊ0 ��������;�����������
	**/
	EVP_CIPHER_CTX_set_padding(pCtx, 0); /* disable padding */

	/**
	int EVP_CipherUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out,
                      int *outl, const unsigned char *in, int inl);
	���� in �������е� inl �ֽڵ����ݲ�����/��������д�� out�����Զ�ε��ô˺�������/�������������ݿ顣
	**/
	EVP_CipherUpdate(pCtx, dst->ptr, &len, data.ptr, data.len);

	/**
	int EVP_CipherFinal_ex(EVP_CIPHER_CTX *ctx, unsigned char *outm, int *outl);
	��� ��������ʣ������ݡ������� EVP_CipherUpdate() ֮����á�
	outm : Ϊ�����������ʣ�ಿ��
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


