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
 * 用c++实现接口类时需要注意一下几点：
 * 1、接口类中不应该声明成员变量，静态变量。
 * 2、可以声明静态常量作为接口的返回值状态，需要在对应的cpp中定义并初始化，
 * 访问时需要使用"接口类型::静态常量名"访问2、定义的接口方法使用virtual 修饰符 
 * 和 “=0” 修饰，表示该方法是纯虚的。
 * 3、因为接口类是无法创建对象的，所以不应该编写构造函数和析构函数
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

