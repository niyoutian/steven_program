#ifndef __HASH_INTERFACE_H__
#define __HASH_INTERFACE_H__
#include "mxDef.h"

/* https://www.iana.org/assignments/ikev2-parameters/ikev2-parameters.xhtml */

/**
 * Hash algorithms as defined for IKEv2 by RFC 7427
 * 7. IANA Considerations
 */
typedef enum 
{
	HASH_SHA1 			= 1,       /* RFC3174 */
	HASH_SHA2_256	    = 2,       /* RFC4634 */
	HASH_SHA2_384		= 3,
	HASH_SHA2_512		= 4,
	/* use private use range for algorithms not defined/permitted by RFC 7427 */
	HASH_UNKNOWN 		= 1024,
	HASH_MD2 			= 1025,
	HASH_MD4			= 1026,
	HASH_MD5 			= 1027,    /* RFC1321 */
	HASH_SHA2_224		= 1028,	   /* RFC3874 */
	HASH_SHA3_224		= 1029,    /* SHA3 ����Keccak�㷨 */
	HASH_SHA3_256		= 1030,
	HASH_SHA3_384		= 1031,
	HASH_SHA3_512		= 1032
}HashType_e;

#define HASH_SIZE_MD2		16
#define HASH_SIZE_MD4		16
#define HASH_SIZE_MD5		16
#define HASH_SIZE_SHA1		20
#define HASH_SIZE_SHA224	28
#define HASH_SIZE_SHA256	32
#define HASH_SIZE_SHA384	48
#define HASH_SIZE_SHA512	64

/**
 * ��c++ʵ�ֽӿ���ʱ��Ҫע��һ�¼��㣺
 * 1���ӿ����в�Ӧ��������Ա��������̬������
 * 2������������̬������Ϊ�ӿڵķ���ֵ״̬����Ҫ�ڶ�Ӧ��cpp�ж��岢��ʼ����
 * ����ʱ��Ҫʹ��"�ӿ�����::��̬������"����2������Ľӿڷ���ʹ��virtual ���η� 
 * �� ��=0�� ���Σ���ʾ�÷����Ǵ���ġ�
 * 3����Ϊ�ӿ������޷���������ģ����Բ�Ӧ�ñ�д���캯������������
 */
class HashInterface
{
public:	
	virtual u32_t getHashType(void) = 0;
	virtual u32_t getHashSize(void) = 0;
	virtual void initHash(void) = 0;
	virtual void calcHash(chunk_t chunk, u8_t *pDigist) = 0;
};

#endif /* __HASH_INTERFACE_H__ */

