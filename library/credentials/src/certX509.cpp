#include <string.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/ossl_typ.h>
#include "mxLog.h"
#include "certX509.h"



certX509::certX509()
{
	mpX509 = NULL;
	mDerEncoding.len = 0;
	mDerEncoding.ptr = NULL;
}


certX509::~certX509()
{
}

/*
https://www.openssl.org/docs/manmaster/man3/PEM_read_X509.html

*/
u32_t certX509::loadX509CertFromPEM(s8_t *filename)
{

	X509* cert = X509_new();
	if (cert == NULL) {
		return STATUS_FAILED;
	}

	BIO* bio_cert = BIO_new_file(filename,"rb");
	
	if (bio_cert == NULL) {
		return STATUS_FAILED;
	}
	PEM_read_bio_X509(bio_cert, &cert, NULL, NULL);

	mpX509 = cert;
	BIO_free(bio_cert);
	
	return STATUS_SUCCESS;
}
/*
The letters i and d in i2d_TYPE stand for "internal" (that is, an internal C structure) 
and "DER" respectively.  So i2d_TYPE converts from internal to DER.

*/
u32_t certX509::transformX509ToDER(void)
{
	mDerEncoding.len = i2d_X509_AUX(mpX509, &mDerEncoding.ptr);
	mxLogFmt(LOG_DEBUG,"len:%u\n",mDerEncoding.len);
	return STATUS_SUCCESS;
}

u32_t certX509::transformDERtoX509(void)
{
	mpX509 = d2i_X509(NULL, (const unsigned char**)&mDerEncoding.ptr, mDerEncoding.len);
	return STATUS_SUCCESS;
}


chunk_t certX509::getSerialNum(void)
{
	ASN1_STRING *asn1 = NULL;
	
	asn1 = X509_get_serialNumber(mpX509);

	if (asn1 != NULL) {
		return chunk_create(ASN1_STRING_data(asn1), ASN1_STRING_length(asn1));
	} 

	return {NULL, 0};
}

u32_t certX509::getCertType(void)
{
	return CERT_X509;
}

/*
https://www.openssl.org/docs/manmaster/man3/X509_get_version.html


#include <openssl/x509.h>

long X509_get_version(const X509 *x);
int X509_set_version(X509 *x, long version);

X509_get_version() returns the numerical value of the version field of certificate x. 
These correspond to the constants X509_VERSION_1, X509_VERSION_2, and X509_VERSION_3.
X509_VERSION_3 has value 2 and X509_VERSION_1 has value 0.
*/
s32_t certX509::getCertVersion(void)
{
	s32_t version = X509_get_version(mpX509);
	
	return version;
}

/*
const X509_ALGOR *X509_get0_tbs_sigalg(const X509 *x);
void X509_ALGOR_get0(const ASN1_OBJECT **paobj, int *pptype,
                     const void **ppval, const X509_ALGOR *alg);
                    
X509_ALGOR_get0() is the inverse of X509_ALGOR_set0(): it returns the algorithm OID in 
*paobj and the associated parameter in *pptype and *ppval from the AlgorithmIdentifier alg.

algorithm：ASN1_OBJECT类型，表明了是何种算法；
parameter：ASN1_TYPE类型，代表该算法需要的参数。ASN1_TYPE类型可以存放任意数据。

OBJ_sha256WithRSAEncryption="\x2A\x86\x48\x86\xF7\x0D\x01\x01\x0B"
*/
chunk_t certX509::getSignatureAlgorithmDER(void)
{
#if 0
	const X509_ALGOR *algor = NULL;
	algor = X509_get0_tbs_sigalg(mpX509);
	
	X509_ALGOR *algor2 = X509_ALGOR_new();
	memcpy(algor2, algor, sizeof(X509_ALGOR));
	s32_t len = i2d_X509_ALGOR(algor2,NULL);
	u8_t *str = (u8_t *)malloc(len);
	len = i2d_X509_ALGOR(algor2,&str);
	X509_ALGOR_free(algor2);
#else
	X509_ALGOR *algor = NULL;
	algor = (X509_ALGOR *)X509_get0_tbs_sigalg(mpX509);
	s32_t len = i2d_X509_ALGOR(algor,NULL);
	u8_t *str = (u8_t *)malloc(len);
	u8_t *p = str;
	len = i2d_X509_ALGOR(algor,&str);   //str指针会变动
#endif

	return {p,len};
}

/*
ASN1_OBJECT * OBJ_nid2obj(int n);
const char *  OBJ_nid2ln(int n);
const char *  OBJ_nid2sn(int n);
int OBJ_obj2nid(const ASN1_OBJECT *o);
int OBJ_ln2nid(const char *ln);
int OBJ_sn2nid(const char *sn);
int OBJ_txt2nid(const char *s);
ASN1_OBJECT * OBJ_txt2obj(const char *s, int no_name);
int OBJ_obj2txt(char *buf, int buf_len, const ASN1_OBJECT *a, int no_name);
int OBJ_cmp(const ASN1_OBJECT *a,const ASN1_OBJECT *b);
ASN1_OBJECT * OBJ_dup(const ASN1_OBJECT *o);
int OBJ_create(const char *oid,const char *sn,const char *ln);
void OBJ_cleanup(void);

OBJ_nid2obj(), OBJ_nid2ln() and OBJ_nid2sn() convert the NID n to an ASN1_OBJECT structure, its long name and its short name respectively, or NULL is an error occurred. 
 

   OBJ_obj2nid(), OBJ_ln2nid(), OBJ_sn2nid() return the corresponding NID for the object o, the long name <ln> or the short name <sn> respectively or NID_undef if an error occurred.
   OBJ_txt2nid() returns NID corresponding to text string <s>. s can be a long name, a short name or the numerical respresentation of an object.
  

OBJ_txt2obj() converts the text string s into an ASN1_OBJECT structure. If no_name is 0 then long names and short names will be interpreted as well as numerical forms. If no_name is 1 only the numerical form is acceptable.
  

OBJ_obj2txt() converts the ASN1_OBJECT a into a textual representation. The representation is written as a null terminated string to buf at most buf_len bytes are written, truncating the result if necessary. The total amount of space required is returned. If no_name is 0 then if the object has a long or short name then that will be used, otherwise the numerical form will be used. If no_name is 1 then the numerical form will always be used.
*/
//https://blog.csdn.net/u011906600/article/details/125642437

const s8_t* certX509::getSignatureAlgorithmLN(void)
{
	const X509_ALGOR *algor = NULL;
	algor = X509_get0_tbs_sigalg(mpX509);
	
	s32_t nid = OBJ_obj2nid(algor->algorithm);  //668
	const s8_t *ln = OBJ_nid2ln(nid);   //"sha256WithRSAEncryption"
/*	
	const s8_t *sn = OBJ_nid2sn(nid);   //"RSA-SHA256"
	char oidstr[80]={0};
	OBJ_obj2txt(oidstr, sizeof(oidstr),algor->algorithm,0);
	*/
	
	return ln;
}

//https://wenku.baidu.com/view/97da4a034835eefdc8d376eeaeaad1f346931163.html
chunk_t certX509::getCertIssuer(void)
{
	X509_NAME *pIssuerName = X509_get_issuer_name(mpX509);
	chunk_t der={NULL,0};
/*	
	int nNameLen = 512;
	s8_t csCommonName[512] = {0};
	nNameLen = X509_NAME_get_text_by_NID(pIssuerName, NID_commonName, csCommonName, nNameLen);
*/

	X509_NAME_get0_der(pIssuerName, (const u8_t **)&der.ptr, (size_t *)&der.len);
	
	return der;
}


void certX509::showCertSubject(void)
{
	X509_NAME *pSubjectName = X509_get_subject_name(mpX509);
	
	int nNameLen = 512;
	s8_t csCommonName[512] = {0};
	nNameLen = X509_NAME_get_text_by_NID(pSubjectName, NID_commonName, csCommonName, nNameLen);
	mxLogFmt(LOG_DEBUG,"CN = %s\n",csCommonName);

	BIO *bio_out = BIO_new_fp(stdout,BIO_NOCLOSE);
	X509_NAME_print(bio_out,pSubjectName, 0);
	BIO_printf(bio_out,"\n");
	BIO_flush(bio_out);
	BIO_free(bio_out);
}


chunk_t certX509::getCertSubject(void)
{
	X509_NAME *pSubjectName = X509_get_subject_name(mpX509);
	chunk_t der={NULL,0};

	X509_NAME_get0_der(pSubjectName, (const u8_t **)&der.ptr, (size_t *)&der.len);
	return der;
}


X509_PUBKEY * certX509::getCertPubKey(void)
{
	X509_PUBKEY * pubkey = X509_get_X509_PUBKEY(mpX509);
	return pubkey;
}

EVP_PKEY * certX509::getCertPubKey2(void)
{
	EVP_PKEY * pubkey = X509_get_pubkey(mpX509);
	//int key_type = EVP_PKEY_type(pubkey->type);
	return pubkey;
}

u32_t certX509::parseCertExtensions(void)
{
	const STACK_OF(X509_EXTENSION) *extensions;
	int i, num;

	extensions = X509_get0_extensions(mpX509);

	if (extensions == NULL) {
		return 0;
	}
	num = sk_X509_EXTENSION_num(extensions);
	
	for (i = 0; i < num; i++){
		X509_EXTENSION *ext;
		bool ok;
	
		ext = sk_X509_EXTENSION_value(extensions, i);
		switch (OBJ_obj2nid(X509_EXTENSION_get_object(ext))) {
			case NID_info_access:
				break;
			case NID_authority_key_identifier:
				break;
			case NID_subject_key_identifier:
				break;
			case NID_subject_alt_name:
				break;
			case NID_issuer_alt_name:
				break;
			case NID_basic_constraints:
				break;
			case NID_key_usage:
				break;
			case NID_ext_key_usage:
				break;
			case NID_crl_distribution_points:
				break;
			case NID_sbgp_ipAddrBlock:
				break;
			default:
				break;

		}
	}

	return 0;
}

