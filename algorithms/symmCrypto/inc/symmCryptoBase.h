#ifndef __SYMM_CRYPTO_BASE_H__
#define __SYMM_CRYPTO_BASE_H__
#include "mxDef.h"

typedef enum 
{
	ENCR_DES_IV64 =            1,
	ENCR_DES =                 2,
	ENCR_3DES =                3,
	ENCR_RC5 =                 4,
	ENCR_IDEA =                5,
	ENCR_CAST =                6,
	ENCR_BLOWFISH =            7,
	ENCR_3IDEA =               8,
	ENCR_DES_IV32 =            9,
	ENCR_NULL =               11,
	ENCR_AES_CBC =            12,
	/** CTR as specified for IPsec (RFC5930/RFC3686), nonce appended to key */
	ENCR_AES_CTR =            13,
	ENCR_AES_CCM_ICV8 =       14,
	ENCR_AES_CCM_ICV12 =      15,
	ENCR_AES_CCM_ICV16 =      16,
	ENCR_AES_GCM_ICV8 =       18,
	ENCR_AES_GCM_ICV12 =      19,
	ENCR_AES_GCM_ICV16 =      20,
	ENCR_NULL_AUTH_AES_GMAC = 21,
	ENCR_CAMELLIA_CBC =       23,
	/* CTR as specified for IPsec (RFC5529), nonce appended to key */
	ENCR_CAMELLIA_CTR =       24,
	ENCR_CAMELLIA_CCM_ICV8 =  25,
	ENCR_CAMELLIA_CCM_ICV12 = 26,
	ENCR_CAMELLIA_CCM_ICV16 = 27,
	ENCR_CHACHA20_POLY1305 =  28,
	ENCR_UNDEFINED =        1024,
	ENCR_DES_ECB =          1025,
	ENCR_SERPENT_CBC =      1026,
	ENCR_TWOFISH_CBC =      1027,
	/* see macros below to handle RC2 (effective) key length */
	ENCR_RC2_CBC =          1028,

}EncryptionType_e;

/**
 * ��c++ʵ�ֽӿ���ʱ��Ҫע��һ�¼��㣺
 * 1���ӿ����в�Ӧ��������Ա��������̬������
 * 2������������̬������Ϊ�ӿڵķ���ֵ״̬����Ҫ�ڶ�Ӧ��cpp�ж��岢��ʼ����
 * ����ʱ��Ҫʹ��"�ӿ�����::��̬������"����2������Ľӿڷ���ʹ��virtual ���η� 
 * �� ��=0�� ���Σ���ʾ�÷����Ǵ���ġ�
 * 3����Ϊ�ӿ������޷���������ģ����Բ�Ӧ�ñ�д���캯������������
 */


class mxSymmEncBase
{
public: 
	virtual u32_t setCryptoType(u32_t encType, u32_t keySize) = 0;
	virtual u32_t setKey(u8_t *pKey, u32_t keyLen) = 0;
	virtual u32_t getKeySize(void) = 0;
	virtual u32_t getIvSize(void) = 0;
	virtual u32_t getBlockSize(void) = 0;
	virtual u32_t encrypt(chunk_t data, chunk_t iv, chunk_t *dst) = 0;
	virtual u32_t decrypt(chunk_t data, chunk_t iv, chunk_t *dst) = 0;
};


#endif /* __SYMM_CRYPTO_BASE_H__ */

