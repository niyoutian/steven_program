#ifndef __CRED_MGR_H__
#define __CRED_MGR_H__
#include "mxDef.h"
#include "certMgr.h"
#include "privateKeyMgr.h"

/* configuration directories and files */
//#define CONFIG_DIR IPSEC_CONFDIR
#define CONFIG_DIR "/usr/local/etc"
#define IPSEC_D_DIR CONFIG_DIR "/ipsec.d"
#define PRIVATE_KEY_DIR IPSEC_D_DIR "/private"
#define CERTIFICATE_DIR IPSEC_D_DIR "/certs"
#define CA_CERTIFICATE_DIR IPSEC_D_DIR "/cacerts"
#define AA_CERTIFICATE_DIR IPSEC_D_DIR "/aacerts"
#define ATTR_CERTIFICATE_DIR IPSEC_D_DIR "/acerts"
#define OCSP_CERTIFICATE_DIR IPSEC_D_DIR "/ocspcerts"
#define CRL_DIR IPSEC_D_DIR "/crls"
#define SECRETS_FILE CONFIG_DIR "/ipsec.secrets"


enum auth_rule_t {
	/** identity to use for IKEv2 authentication exchange, identification_t* */
	AUTH_RULE_IDENTITY,
	/** if TRUE don't send IDr as initiator, but verify the identity after
	 * receiving IDr (but also verify it against subjectAltNames), bool */
	AUTH_RULE_IDENTITY_LOOSE,
	/** authentication class, auth_class_t */
	AUTH_RULE_AUTH_CLASS,
	/** AAA-backend identity for EAP methods supporting it, identification_t* */
	AUTH_RULE_AAA_IDENTITY,
	/** EAP identity to use within EAP-Identity exchange, identification_t* */
	AUTH_RULE_EAP_IDENTITY,
	/** EAP type to propose for peer authentication, eap_type_t */
	AUTH_RULE_EAP_TYPE,
	/** EAP vendor for vendor specific type, uint32_t */
	AUTH_RULE_EAP_VENDOR,
	/** XAUTH backend name to use, char* */
	AUTH_RULE_XAUTH_BACKEND,
	/** XAuth identity to use or require, identification_t* */
	AUTH_RULE_XAUTH_IDENTITY,
	/** certificate authority, certificate_t* */
	AUTH_RULE_CA_CERT,
	/** intermediate certificate in trustchain, certificate_t* */
	AUTH_RULE_IM_CERT,
	/** subject certificate, certificate_t* */
	AUTH_RULE_SUBJECT_CERT,
	/** result of a CRL validation, cert_validation_t */
	AUTH_RULE_CRL_VALIDATION,
	/** result of a OCSP validation, cert_validation_t */
	AUTH_RULE_OCSP_VALIDATION,
	/** CRL/OCSP validation is disabled, bool */
	AUTH_RULE_CERT_VALIDATION_SUSPENDED,
	/** subject is member of a group, identification_t*
	 * The group membership constraint is fulfilled if the subject is member of
	 * one group defined in the constraints. */
	AUTH_RULE_GROUP,
	/** required RSA public key strength, u_int in bits */
	AUTH_RULE_RSA_STRENGTH,
	/** required ECDSA public key strength, u_int in bits */
	AUTH_RULE_ECDSA_STRENGTH,
	/** required BLISS public key strength, u_int in bits */
	AUTH_RULE_BLISS_STRENGTH,
	/** required signature scheme, signature_params_t* */
	AUTH_RULE_SIGNATURE_SCHEME,
	/** required signature scheme for IKE authentication, signature_params_t* */
	AUTH_RULE_IKE_SIGNATURE_SCHEME,
	/** certificatePolicy constraint, numerical OID as char* */
	AUTH_RULE_CERT_POLICY,

	/** intermediate certificate, certificate_t* */
	AUTH_HELPER_IM_CERT,
	/** subject certificate, certificate_t* */
	AUTH_HELPER_SUBJECT_CERT,
	/** Hash and URL of a intermediate certificate, char* */
	AUTH_HELPER_IM_HASH_URL,
	/** Hash and URL of a end-entity certificate, char* */
	AUTH_HELPER_SUBJECT_HASH_URL,
	/** revocation certificate (CRL, OCSP), certificate_t* */
	AUTH_HELPER_REVOCATION_CERT,
	/** attribute certificate for authorization decisions, certificate_t */
	AUTH_HELPER_AC_CERT,

	/** helper to determine the number of elements in this enum */
	AUTH_RULE_MAX,
};


//pkcs1_public_key_load  pkcs1_public_key_load
class credMgr
{
public:
	credMgr();
	~credMgr();
	static credMgr* getInstance(void);
	void loadCerts(void);
	void loadSecrets(void);
	
	certMgr* getCertMgr(void);
	privateKeyMgr* getPrivateKeyMgr(void);
private:
	static credMgr* mpInstance;
	u32_t loadCertDir(s8_t *path, u32_t certType, u32_t certFlag);
	u32_t loadCertByType(s8_t *file, u32_t certType, u32_t certFlag);
	u32_t getKeyType(chunk_t *token, u32_t &KeyType);
	u32_t loadPriKeyByType(s8_t *file, u32_t KeyType, chunk_t secret);
	
	certMgr *mpCertMgr;
	privateKeyMgr *mpPrivateKeyMgr;
};

#endif

