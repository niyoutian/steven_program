#ifndef __CERT_X509_H__
#define __CERT_X509_H__
#include <openssl/x509.h>
#include "mxDef.h"
#include "mxStatus.h"
#include "certInterface.h"

typedef enum eKeyidType {
	/** SHA1 fingerprint over subjectPublicKeyInfo */
	KEYID_PUBKEY_INFO_SHA1 = 0,
	/** SHA1 fingerprint over subjectPublicKey */
	KEYID_PUBKEY_SHA1,
} eKeyidType_t;

#define EVP_MAX_MD_SIZE                 64/* longest known is SHA512 */

class certX509 : public certInterface
{
public:
	certX509();
	~certX509();
	u32_t loadX509CertFromPEM(s8_t *filename);
	u32_t loadX509CertFromDER(s8_t *filename);
	u32_t transformX509ToDER(void);
	u32_t transformDERtoX509(void);
	chunk_t getSerialNum(void);
	u32_t getCertType(void);
	s32_t getCertVersion(void);
	chunk_t getSignatureAlgorithmDER(void);
	const s8_t* getSignatureAlgorithmLN(void);
	chunk_t getCertIssuer(void);
	void showCertSubject(void);
	chunk_t getCertSubjectDER(void);
	chunk_t getCertSubjectString(void);
	X509_PUBKEY* getCertPubKey(void);
	EVP_PKEY* getCertPubKey2(void);
	u32_t parseCertExtensions(void);
	u32_t getFingerprint(u32_t type, chunk_t& fp);
private:
	bool parseBasicConstraintsExt(X509_EXTENSION *ext);
	bool parseKeyUsageExt(X509_EXTENSION *ext);
	bool parseExtKeyUsageExt(X509_EXTENSION *ext);
	bool parseAuthKeyIdentifierExt(X509_EXTENSION *ext);
	bool parseSubjectKeyIdentifierExt(X509_EXTENSION *ext);

	u32_t calculatePublicKeyInfoHash(const s8_t *name, chunk_t& fp);
	u32_t calculatePublicKeyHash(const s8_t *name, chunk_t& fp);
	X509 *mpX509;
	chunk_t mDerEncoding;   /* DER encoded certificate */
	u32_t mCertType;
	u32_t  mFlags;
};


#endif

