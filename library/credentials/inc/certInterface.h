#ifndef __CERT_INTERFACE_H__
#define __CERT_INTERFACE_H__
#include "mxDef.h"
//#include "mxStatus.h"

/* constraints are currently restricted to the range 0..127 */
#define X509_NO_CONSTRAINT	255


/**
 * Kind of a certificate_t
 */
enum certificate_type_t {
	/** just any certificate */
	CERT_ANY,
	/** X.509 certificate */
	CERT_X509,
	/** X.509 certificate revocation list */
	CERT_X509_CRL,
	/** X.509 online certificate status protocol request */
	CERT_X509_OCSP_REQUEST,
	/** X.509 online certificate status protocol response */
	CERT_X509_OCSP_RESPONSE,
	/** X.509 attribute certificate */
	CERT_X509_AC,
	/** trusted, preinstalled public key */
	CERT_TRUSTED_PUBKEY,
	/** PKCS#10 certificate request */
	CERT_PKCS10_REQUEST,
	/** PGP certificate */
	CERT_GPG,
};

enum certificate_subtype_t {
	/** subject certificate */
	CERT_SUBTYPE_SUBJECT,
	/** intermediate certificate in trustchain */
	CERT_SUBTYPE_IM,
	CERT_SUBTYPE_CA
};


/**
 * Result of a certificate validation.
 *
 * Order of values is relevant, sorted from good to bad.
 */
enum cert_validation_t {
	/** certificate has been validated successfully */
	VALIDATION_GOOD = 0,
	/** validation has been skipped due to missing validation information */
	VALIDATION_SKIPPED,
	/** certificate has been validated, but check based on stale information */
	VALIDATION_STALE,
	/** validation failed due to a processing error */
	VALIDATION_FAILED,
	/** certificate is on hold (i.e. temporary revokation) */
	VALIDATION_ON_HOLD,
	/** certificate has been revoked */
	VALIDATION_REVOKED,
};

/**
 * X.509 certificate flags.
 */
enum x509_flag_t {
	/** cert has no constraints */
	X509_NONE =	               0,
	/** cert has CA constraint */
	X509_CA =                 (1<<0),
	/** cert has AA constraint */
	X509_AA =                 (1<<1),
	/** cert has OCSP signer constraint */
	X509_OCSP_SIGNER =        (1<<2),
    /** cert has either CA, AA or OCSP constraint */
	X509_ANY = X509_CA | X509_AA | X509_OCSP_SIGNER,
	/** cert has serverAuth key usage */
	X509_SERVER_AUTH =        (1<<3),
	/** cert has clientAuth key usage */
	X509_CLIENT_AUTH =        (1<<4),
	/** cert is self-signed */
	X509_SELF_SIGNED =        (1<<5),
	/** cert has an ipAddrBlocks extension */
	X509_IP_ADDR_BLOCKS =     (1<<6),
	/** cert has CRL sign key usage */
	X509_CRL_SIGN =           (1<<7),
	/** cert has iKEIntermediate key usage */
	X509_IKE_INTERMEDIATE =   (1<<8),
	/** cert has Microsoft Smartcard Logon usage */
	X509_MS_SMARTCARD_LOGON = (1<<9),
};



class certInterface
{
public:
	certInterface();
	~certInterface();
	virtual u32_t getCertType(void) = 0;
private:
};

#endif

