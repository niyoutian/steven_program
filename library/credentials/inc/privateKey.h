#ifndef __PRIVATE_KEY_H__
#define __PRIVATE_KEY_H__
#include "mxDef.h"

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
 * Abstract private key interface.
 */
class privateKey
{
public: 
	privateKey();
	~privateKey();

	virtual u32_t getKeyType(void) = 0;
private:

};


#endif /* __PRIVATE_KEY_H__ */

