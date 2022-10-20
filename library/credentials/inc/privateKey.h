#ifndef __PRIVATE_KEY_H__
#define __PRIVATE_KEY_H__
#include "mxDef.h"
#include "mxStatus.h"
#include "chunk.h"

/**
 * Type of a key pair, the used crypto system
 */
enum key_type_t {
	/** key type wildcard */
	KEY_ANY     = 0,
	/** RSA crypto system as in PKCS#1 */
	KEY_RSA     = 1,
	/** ECDSA as in ANSI X9.62 */
	KEY_ECDSA   = 2,
	/** DSA */
	KEY_DSA     = 3,
	/** Ed25519 PureEdDSA instance as in RFC 8032 */
	KEY_ED25519 = 4,
	/** Ed448   PureEdDSA instance as in RFC 8032 */
	KEY_ED448   = 5,
	/** BLISS */
	KEY_BLISS = 6,
};

/**
 * Signature scheme for signature creation
 *
 * EMSA-PKCS1 signatures are defined in PKCS#1 standard.
 * A prepended ASN.1 encoded digestInfo field contains the
 * OID of the used hash algorithm.
 */
enum signature_scheme_t {
	/** Unknown signature scheme                                       */
	SIGN_UNKNOWN,
	/** EMSA-PKCS1_v1.5 signature over digest without digestInfo       */
	SIGN_RSA_EMSA_PKCS1_NULL,
	/** EMSA-PKCS1_v1.5 signature as in PKCS#1 using RSA and MD5       */
	SIGN_RSA_EMSA_PKCS1_MD5,
	/** EMSA-PKCS1_v1.5 signature as in PKCS#1 using RSA and SHA-1     */
	SIGN_RSA_EMSA_PKCS1_SHA1,
	/** EMSA-PKCS1_v1.5 signature as in PKCS#1 using RSA and SHA-2_224 */
	SIGN_RSA_EMSA_PKCS1_SHA2_224,
	/** EMSA-PKCS1_v1.5 signature as in PKCS#1 using RSA and SHA-2_256 */
	SIGN_RSA_EMSA_PKCS1_SHA2_256,
	/** EMSA-PKCS1_v1.5 signature as in PKCS#1 using RSA and SHA-2_384 */
	SIGN_RSA_EMSA_PKCS1_SHA2_384,
	/** EMSA-PKCS1_v1.5 signature as in PKCS#1 using RSA and SHA-2_512 */
	SIGN_RSA_EMSA_PKCS1_SHA2_512,
	/** EMSA-PKCS1_v1.5 signature as in PKCS#1 using RSA and SHA-3_224 */
	SIGN_RSA_EMSA_PKCS1_SHA3_224,
	/** EMSA-PKCS1_v1.5 signature as in PKCS#1 using RSA and SHA-3_256 */
	SIGN_RSA_EMSA_PKCS1_SHA3_256,
	/** EMSA-PKCS1_v1.5 signature as in PKCS#1 using RSA and SHA-3_384 */
	SIGN_RSA_EMSA_PKCS1_SHA3_384,
	/** EMSA-PKCS1_v1.5 signature as in PKCS#1 using RSA and SHA-3_512 */
	SIGN_RSA_EMSA_PKCS1_SHA3_512,
	/** EMSA-PSS signature as in PKCS#1 using RSA                      */
	SIGN_RSA_EMSA_PSS,
	/** ECDSA with SHA-1 using DER encoding as in RFC 3279             */
	SIGN_ECDSA_WITH_SHA1_DER,
	/** ECDSA with SHA-256 using DER encoding as in RFC 3279           */
	SIGN_ECDSA_WITH_SHA256_DER,
	/** ECDSA with SHA-384 using DER encoding as in RFC 3279           */
	SIGN_ECDSA_WITH_SHA384_DER,
	/** ECDSA with SHA-1 using DER encoding as in RFC 3279             */
	SIGN_ECDSA_WITH_SHA512_DER,
	/** ECDSA over precomputed digest, signature as in RFC 4754        */
	SIGN_ECDSA_WITH_NULL,
	/** ECDSA on the P-256 curve with SHA-256 as in RFC 4754           */
	SIGN_ECDSA_256,
	/** ECDSA on the P-384 curve with SHA-384 as in RFC 4754           */
	SIGN_ECDSA_384,
	/** ECDSA on the P-521 curve with SHA-512 as in RFC 4754           */
	SIGN_ECDSA_521,
	/** PureEdDSA on Curve25519 as in draft-ietf-curdle-pkix (RFC TBA) */
	SIGN_ED25519,
	/** PureEdDSA on Curve448 as in draft-ietf-curdle-pkix (RFC TBA)   */
	SIGN_ED448,
	/** BLISS with SHA-2_256                                           */
	SIGN_BLISS_WITH_SHA2_256,
	/** BLISS with SHA-2_384                                           */
	SIGN_BLISS_WITH_SHA2_384,
	/** BLISS with SHA-2_512                                           */
	SIGN_BLISS_WITH_SHA2_512,
	/** BLISS with SHA-3_256                                           */
	SIGN_BLISS_WITH_SHA3_256,
	/** BLISS with SHA-3_384                                           */
	SIGN_BLISS_WITH_SHA3_384,
	/** BLISS with SHA-3_512                                           */
	SIGN_BLISS_WITH_SHA3_512,
};




/**
 * Abstract private key interface.
 */
class privateKey
{
public: 
	privateKey();
	~privateKey();

	virtual u32_t getKeyType(void) = 0;
	virtual u32_t loadPriKeyFromPEM(s8_t *filename, chunk_t secret) = 0;
	virtual u32_t loadPriKeyFromDER(s8_t *filename) = 0;
	virtual u32_t calcSignature(u32_t sign_type, chunk_t data, chunk_t *signature) = 0;
private:

};


#endif /* __PRIVATE_KEY_H__ */

