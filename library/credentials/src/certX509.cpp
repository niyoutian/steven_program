#include <string.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/ossl_typ.h>
#include <openssl/x509v3.h>
#include "mxLog.h"
#include "certX509.h"
#include "asn1Parser.h"



#define X509_OBJ_TBS_CERTIFICATE				 1
#define X509_OBJ_VERSION						 3
#define X509_OBJ_SERIAL_NUMBER					 4
#define X509_OBJ_SIG_ALG						 5
#define X509_OBJ_ISSUER							 6
#define X509_OBJ_NOT_BEFORE						 8
#define X509_OBJ_NOT_AFTER						 9
#define X509_OBJ_SUBJECT						10
#define X509_OBJ_SUBJECT_PUBLIC_KEY_INFO		11
#define X509_OBJ_OPTIONAL_EXTENSIONS			16
#define X509_OBJ_EXTN_ID						19
#define X509_OBJ_CRITICAL						20
#define X509_OBJ_EXTN_VALUE						21
#define X509_OBJ_ALGORITHM						24
#define X509_OBJ_SIGNATURE						25


/**
 * ASN.1 definition of an X.509v3 x509_cert
 */
static const asn1Object_t certObjects[] = {
	{ 0, "x509",					ASN1_PARSER_SEQUENCE,		ASN1_OBJ			}, /*  0 */
	{ 1,   "tbsCertificate",		ASN1_PARSER_SEQUENCE,		ASN1_OBJ			}, /*  1 */
	{ 2,     "DEFAULT v1",			ASN1_PARSER_CONTEXT_C_0,	ASN1_DEF			}, /*  2 */
	{ 3,       "version",			ASN1_PARSER_INTEGER,		ASN1_BODY			}, /*  3 */
	{ 2,     "serialNumber",		ASN1_PARSER_INTEGER,		ASN1_BODY			}, /*  4 */
	{ 2,     "signature",			ASN1_PARSER_EOC,			ASN1_RAW			}, /*  5 */
	{ 2,     "issuer",				ASN1_PARSER_SEQUENCE,		ASN1_OBJ			}, /*  6 */
	{ 2,     "validity",			ASN1_PARSER_SEQUENCE,		ASN1_NONE			}, /*  7 */
	{ 3,       "notBefore",			ASN1_PARSER_EOC,			ASN1_RAW			}, /*  8 */
	{ 3,       "notAfter",			ASN1_PARSER_EOC,			ASN1_RAW			}, /*  9 */
	{ 2,     "subject",				ASN1_PARSER_SEQUENCE,		ASN1_OBJ			}, /* 10 */
	{ 2,     "subjectPublicKeyInfo",ASN1_PARSER_SEQUENCE,		ASN1_RAW			}, /* 11 */
	{ 2,     "issuerUniqueID",		ASN1_PARSER_CONTEXT_C_1,	ASN1_OPT			}, /* 12 */
	{ 2,     "end opt",				ASN1_PARSER_EOC,			ASN1_END			}, /* 13 */
	{ 2,     "subjectUniqueID",		ASN1_PARSER_CONTEXT_C_2,	ASN1_OPT			}, /* 14 */
	{ 2,     "end opt",				ASN1_PARSER_EOC,			ASN1_END			}, /* 15 */
	{ 2,     "optional extensions",	ASN1_PARSER_CONTEXT_C_3,	ASN1_OPT			}, /* 16 */
	{ 3,       "extensions",		ASN1_PARSER_SEQUENCE,		ASN1_LOOP			}, /* 17 */
	{ 4,         "extension",		ASN1_PARSER_SEQUENCE,		ASN1_NONE			}, /* 18 */
	{ 5,           "extnID",		ASN1_PARSER_OID,			ASN1_BODY			}, /* 19 */
	{ 5,           "critical",		ASN1_PARSER_BOOLEAN,		ASN1_DEF|ASN1_BODY	}, /* 20 */
	{ 5,           "extnValue",		ASN1_PARSER_OCTET_STRING,	ASN1_BODY			}, /* 21 */
	{ 3,       "end loop",			ASN1_PARSER_EOC,			ASN1_END			}, /* 22 */
	{ 2,     "end opt",				ASN1_PARSER_EOC,			ASN1_END			}, /* 23 */
	{ 1,   "signatureAlgorithm",	ASN1_PARSER_EOC,			ASN1_RAW			}, /* 24 */
	{ 1,   "signatureValue",		ASN1_PARSER_BIT_STRING,	    ASN1_BODY			}, /* 25 */
	{ 0, "exit",					ASN1_PARSER_EOC,			ASN1_EXIT			}
};



certX509::certX509()
{
	mpX509 = NULL;
	mDerEncoding.len = 0;
	mDerEncoding.ptr = NULL;
	mCertType = CERT_ANY;
	mCertFlags = X509_NONE;
	mCertSubType = 0;
}


certX509::~certX509()
{
	if (mDerEncoding.ptr != NULL) {
		free(mDerEncoding.ptr);
		mDerEncoding.ptr = NULL;
	}

	if (mpX509 != NULL) {
		X509_free(mpX509);
		mpX509 = NULL;
	}
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
	transformX509ToDER();
	parseCertExtensions();
	return STATUS_SUCCESS;
}

u32_t certX509::loadX509CertFromDER(s8_t *filename)
{

	X509* cert = X509_new();
	if (cert == NULL) {
		return STATUS_FAILED;
	}

	BIO* bio_cert = BIO_new_file(filename,"rb");
	
	if (bio_cert == NULL) {
		return STATUS_FAILED;
	}
	cert = d2i_X509_bio(bio_cert, NULL);

	mpX509 = cert;
	BIO_free(bio_cert);
	transformX509ToDER();
	parseCertExtensions();
	return STATUS_SUCCESS;
}


/*
The letters i and d in i2d_TYPE stand for "internal" (that is, an internal C structure) 
and "DER" respectively.  So i2d_TYPE converts from internal to DER.

*/
u32_t certX509::transformX509ToDER(void)
{
	mDerEncoding.len = i2d_X509_AUX(mpX509, &mDerEncoding.ptr);
	//mxLogFmt(LOG_DEBUG,"len:%u\n",mDerEncoding.len);
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
	return mCertType;
}

u32_t certX509::getCertFlag(void)
{
	return mCertFlags;
}

void certX509::setCertFlag(u32_t flag)
{
	mCertFlags |= flag;
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


chunk_t certX509::getCertSubjectDER(void)
{
	X509_NAME *pSubjectName = X509_get_subject_name(mpX509);
	chunk_t der={NULL,0};

	X509_NAME_get0_der(pSubjectName, (const u8_t **)&der.ptr, (size_t *)&der.len);
	return der;
}

/*
返回值 chunk_t由调用者释放
*/
chunk_t certX509::getCertSubjectString(void)
{
	X509_NAME *pSubjectName = X509_get_subject_name(mpX509);
	s32_t entriesNum  = X509_NAME_entry_count(pSubjectName);
	X509_NAME_ENTRY *name_entry = NULL;
	ASN1_STRING *entry_value = NULL;
	s32_t Nid = 0;
	s8_t str[128]={0};
	s32_t str_len = 0;
	//循环读取个条目信息
	for(s32_t i = 0; i < entriesNum; i++) {
		name_entry = X509_NAME_get_entry(pSubjectName, i);
		Nid = OBJ_obj2nid(X509_NAME_ENTRY_get_object(name_entry));
		entry_value = X509_NAME_ENTRY_get_data(name_entry);
		switch(Nid) {
			case NID_countryName://国家C
				str_len += snprintf(str+str_len, sizeof(str)-str_len,"C = %s, ",entry_value->data);
				break;
			case NID_stateOrProvinceName://省ST
				printf("issuer 's ST:%s\n",entry_value->data);
				str_len += snprintf(str+str_len, sizeof(str)-str_len,"ST = %s, ",entry_value->data);
				break;
			case NID_localityName://地区L
				str_len += snprintf(str+str_len, sizeof(str)-str_len,"L = %s, ",entry_value->data);
				break;
			case NID_organizationName://组织O
				str_len += snprintf(str+str_len, sizeof(str)-str_len,"O = %s, ",entry_value->data);
				break;
			case NID_organizationalUnitName://单位OU
				str_len += snprintf(str+str_len, sizeof(str)-str_len,"OU = %s, ",entry_value->data);
				break;
			case NID_commonName://通用名CN
				str_len += snprintf(str+str_len, sizeof(str)-str_len,"CN = %s, ",entry_value->data);
				break;						
			case NID_pkcs9_emailAddress://Mail
				str_len += snprintf(str+str_len, sizeof(str)-str_len,"M = %s, ",entry_value->data);
				break;
			default:
				break;
		}
	}
	chunk_t chunk = {NULL, 0};
	
	if (str_len > 2) {
		str_len = str_len - 2; //remove ", "
		str[str_len] = 0;
		chunk.ptr = (u8_t*)malloc(str_len+1);
		strncpy((s8_t *)chunk.ptr, str,str_len);
		chunk.len = str_len;
	}
	
	return chunk;
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
			case NID_basic_constraints://nid=87
				ok = parseBasicConstraintsExt(ext);
				break;
			case NID_key_usage:    //nid=83
				ok = parseKeyUsageExt(ext);
				break;
			case NID_info_access:
				break;
			case NID_authority_key_identifier: //nid=90
				ok = parseAuthKeyIdentifierExt(ext);
				break;
			case NID_subject_key_identifier: //nid=82
				ok = parseSubjectKeyIdentifierExt(ext);
				break;
			case NID_subject_alt_name:
				break;
			case NID_issuer_alt_name:
				break;
			case NID_ext_key_usage:
				ok = parseExtKeyUsageExt(ext);
				break;
			case NID_crl_distribution_points:
				break;
			case NID_sbgp_ipAddrBlock:
				break;
			default:
				ok = X509_EXTENSION_get_critical(ext) == 0;
				if (!ok) {
					char buf[80] = "";
					OBJ_obj2txt(buf, sizeof(buf),X509_EXTENSION_get_object(ext), 0);
					mxLogFmt(LOG_DEBUG,"found unsupported critical X.509 extension: %s\n",buf);
				}

				break;

		}
		if (!ok) {
			return false;
		}
	}

	return 0;
}

/**
 * parse basic constraints
 * OBJ_basic_constraints="\x55\x1D\x13"
 000001f0  xx xx xx a3 81 b2 30 81	af 30 12 06 03 55 1d 13  |......0..0...U..|
 00000200  01 01 ff 04 08 30 06 01	01 ff 02 01 01 30 0b 06  |.....0.......0..|
 00000210  03 55 1d 0f 04 04 03 02	01 06 30 1d 06 03 55 1d  |.U........0...U.|
 00000220  0e 04 16 04 14 5d a7 dd	70 06 51 32 7e e7 b6 6d  |.....]..p.Q2~..m|
 00000230  b3 b5 e5 e0 60 ea 2e 4d	ef 30 6d 06 03 55 1d 23  |....`..M.0m..U.#|
 00000240  04 66 30 64 80 14 5d a7	dd 70 06 51 32 7e e7 b6  |.f0d..]..p.Q2~..|
 00000250  6d b3 b5 e5 e0 60 ea 2e	4d ef a1 49 a4 47 30 45  |m....`..M..I.G0E|
 00000260  31 0b 30 09 06 03 55 04	06 13 02 43 48 31 19 30  |1.0...U....CH1.0|
 00000270  17 06 03 55 04 0a 13 10	4c 69 6e 75 78 20 73 74  |...U....Linux st|
 00000280  72 6f 6e 67 53 77 61 6e	31 1b 30 19 06 03 55 04  |rongSwan1.0...U.|
 00000290  03 13 12 73 74 72 6f 6e	67 53 77 61 6e 20 52 6f  |...strongSwan Ro|
 000002a0  6f 74 20 43 41 82 01 00 

 499:d=2  hl=3 l= 178 cons: cont [ 3 ]		  
 502:d=3  hl=3 l= 175 cons: SEQUENCE		  
 505:d=4  hl=2 l=  18 cons: SEQUENCE		  
 507:d=5  hl=2 l=	3 prim: OBJECT			  :X509v3 Basic Constraints
 512:d=5  hl=2 l=	1 prim: BOOLEAN 		  :255
 515:d=5  hl=2 l=	8 prim: OCTET STRING	  
	 0000 - 30 06 01 01 ff 02 01 01-						  0.......
 525:d=4  hl=2 l=  11 cons: SEQUENCE		  
 527:d=5  hl=2 l=	3 prim: OBJECT			  :X509v3 Key Usage
 532:d=5  hl=2 l=	4 prim: OCTET STRING	  
	 0000 - 03 02 01 06 									  ....
 538:d=4  hl=2 l=  29 cons: SEQUENCE		  
 540:d=5  hl=2 l=	3 prim: OBJECT			  :X509v3 Subject Key Identifier
 545:d=5  hl=2 l=  22 prim: OCTET STRING	  
	 0000 - 04 14 5d a7 dd 70 06 51-32 7e e7 b6 6d b3 b5 e5   ..]..p.Q2~..m...
	 0010 - e0 60 ea 2e 4d ef								  .`..M.
 569:d=4  hl=2 l= 109 cons: SEQUENCE		  
 571:d=5  hl=2 l=	3 prim: OBJECT			  :X509v3 Authority Key Identifier
 576:d=5  hl=2 l= 102 prim: OCTET STRING	  
	 0000 - 30 64 80 14 5d a7 dd 70-06 51 32 7e e7 b6 6d b3   0d..]..p.Q2~..m.
	 0010 - b5 e5 e0 60 ea 2e 4d ef-a1 49 a4 47 30 45 31 0b   ...`..M..I.G0E1.
	 0020 - 30 09 06 03 55 04 06 13-02 43 48 31 19 30 17 06   0...U....CH1.0..
	 0030 - 03 55 04 0a 13 10 4c 69-6e 75 78 20 73 74 72 6f   .U....Linux stro
	 0040 - 6e 67 53 77 61 6e 31 1b-30 19 06 03 55 04 03 13   ngSwan1.0...U...
	 0050 - 12 73 74 72 6f 6e 67 53-77 61 6e 20 52 6f 6f 74   .strongSwan Root
	 0060 - 20 43 41 82 01 00								   CA...

 */
bool certX509::parseBasicConstraintsExt(X509_EXTENSION *ext)
{
	u8_t pathlen = 0;
	BASIC_CONSTRAINTS *constraints;

	constraints = (BASIC_CONSTRAINTS*)X509V3_EXT_d2i(ext);
	if (constraints) {
		if (constraints->ca) {
			mCertFlags |= X509_CA;
		}
		if (constraints->pathlen) {

			pathlen = ASN1_INTEGER_get(constraints->pathlen);
			pathlen = (pathlen >= 0 && pathlen < 128) ? pathlen : X509_NO_CONSTRAINT;
		}
		BASIC_CONSTRAINTS_free(constraints);
		return true;
	}
	return false;
}

/**
 * parse key usage
  00000200  xx xx xx xx xx xx xx xx xx xx xx xx xx 30 0b 06  |.....0.......0..|
  00000210  03 55 1d 0f 04 04 03 02	01 06 xx xx xx xx xx xx  |.U........0...U.|
  OBJ_key_usage="\x55\x1D\x0F"
  字节串 OCTET STRING
  标识符为：0x04
  字节串在Contents中按照字节串的值的原始顺序，使用0或多个字节，来表示字节串的值。
# define X509v3_KU_KEY_CERT_SIGN         0x0004
# define X509v3_KU_CRL_SIGN              0x0002
# define X509v3_KU_ENCIPHER_ONLY         0x0001

 */
bool certX509::parseKeyUsageExt(X509_EXTENSION *ext)
{
	ASN1_BIT_STRING *usage = NULL;

	usage = (ASN1_BIT_STRING *)X509V3_EXT_d2i(ext);
	if (usage){
		if (usage->length > 0){
			int flags = usage->data[0];
			
			if (usage->length > 1){
				flags |= usage->data[1] << 8;
			}
			if (flags & X509v3_KU_CRL_SIGN){
				this->mCertFlags |= X509_CRL_SIGN;
				mxLogFmt(LOG_DEBUG,"X509v3 Key Usage:CRL Sign\n");
			}
			if (flags & X509v3_KU_KEY_CERT_SIGN){
				/* we use the caBasicContraint, MUST be set */
				mxLogFmt(LOG_DEBUG,"X509v3 Key Usage:Certificate Sign\n");
			}
		}
		ASN1_BIT_STRING_free(usage);
		return true;
	}
	return false;
}

/**
 * Parse ExtendedKeyUsage
 */
bool certX509::parseExtKeyUsageExt(X509_EXTENSION *ext)
{
	EXTENDED_KEY_USAGE *usage = NULL;
	int i;

	usage = (EXTENDED_KEY_USAGE *)X509V3_EXT_d2i(ext);
	if (usage)
	{
		for (i = 0; i < sk_ASN1_OBJECT_num(usage); i++)
		{
			switch (OBJ_obj2nid(sk_ASN1_OBJECT_value(usage, i)))
			{
				case NID_server_auth:
					this->mCertFlags |= X509_SERVER_AUTH;
					break;
				case NID_client_auth:
					this->mCertFlags |= X509_CLIENT_AUTH;
					break;
				case NID_OCSP_sign:
					this->mCertFlags |= X509_OCSP_SIGNER;
					break;
				default:
					break;
			}
		}
		sk_ASN1_OBJECT_pop_free(usage, ASN1_OBJECT_free);
		return true;
	}
	return false;
}

/**
 * Parse authorityKeyIdentifier extension
	 struct AUTHORITY_KEYID_st {
	    ASN1_OCTET_STRING *keyid;
	    GENERAL_NAMES *issuer;
	    ASN1_INTEGER *serial;
	};
 */
bool certX509::parseAuthKeyIdentifierExt(X509_EXTENSION *ext)
{
	AUTHORITY_KEYID *keyid;

	keyid = (AUTHORITY_KEYID*)X509V3_EXT_d2i(ext);
	if (keyid)
	{
		chunk_t authKid;
		authKid = chunk_create(ASN1_STRING_data(keyid->keyid), ASN1_STRING_length(keyid->keyid));
		mxLogFmt(LOG_DEBUG,"len:%d\n",ASN1_STRING_length(keyid->keyid));
		AUTHORITY_KEYID_free(keyid);
		return true;
	}
	return false;
}

/**
 * Parse subjectKeyIdentifier extension
 */
bool certX509::parseSubjectKeyIdentifierExt(X509_EXTENSION *ext)
{
	ASN1_STRING *asn1 = X509_EXTENSION_get_data(ext);
	chunk_t subjetKid;
	subjetKid = chunk_create(ASN1_STRING_data(asn1), ASN1_STRING_length(asn1));


	return true;
}

/*
ASN1_BIT_STRING *pubKey2 = X509_get0_pubkey_bitstr(certificateX509);
X509_get0_pubkey_bitstr(mpX509); 再做摘要
EVP_KEY *pubkey = X509_get_pubkey(mpX509);
*/
u32_t certX509::getFingerprint(u32_t type, chunk_t& fp)
{

	
	switch (type) {
		case KEYID_PUBKEY_INFO_SHA1:
			calculatePublicKeyInfoHash(NULL,fp);
			break;
		case KEYID_PUBKEY_SHA1:
			calculatePublicKeyHash(NULL,fp);
			break;
		default:
			return STATUS_FAILED;
	}
	return STATUS_SUCCESS;
}

/*
205:d=2  hl=4 l= 290 cons: SEQUENCE 		 
209:d=3  hl=2 l=  13 cons: SEQUENCE 		 
211:d=4  hl=2 l=   9 prim: OBJECT			 :rsaEncryption
222:d=4  hl=2 l=   0 prim: NULL 			 
224:d=3  hl=4 l= 271 prim: BIT STRING		 
	0000 - 00 30 82 01 0a 02 82 01-01 00 bf f2 5f 62 ea 3d	 .0.........._b.=
	0010 - 56 6e 58 b3 c8 7a 49 ca-f3 ac 61 cf a9 63 77 73	 VnX..zI...a..cws
	0020 - 4d 84 2d b3 f8 fd 6e a0-23 f7 b0 13 2e 66 26 50	 M.-...n.#....f&P
	0030 - 12 31 73 86 72 9c 6d 7c-42 7a 8d 9f 16 7b e1 38	 .1s.r.m|Bz...{.8
	0040 - e8 eb ae 2b 12 b9 59 33-ba ef 36 a3 15 c3 dd f2	 ...+..Y3..6.....
	0050 - 24 ce e4 bb 9b d5 78 13-5d 04 67 38 26 29 62 1f	 $.....x.].g8&)b.
	0060 - f9 6b 8d 45 f6 e0 02 e5-08 36 62 dc e1 81 80 5c	 .k.E.....6b....\
	0070 - 14 0b 3f 2c e9 3f 83 ae-e3 c8 61 cf f6 10 a3 9f	 ..?,.?....a.....
	0080 - 01 89 cb 3a 3c 7c b9 bf-7e 2a 09 54 4e 21 70 ef	 ...:<|..~*.TN!p.
	0090 - aa 18 fd d4 ff 20 fa 94-be 17 6d 7f ec ff 82 1f	 ..... ....m.....
	00a0 - 68 d1 71 52 04 1d 9b 46-f0 cf cf c1 e4 cf 43 de	 h.qR...F......C.
	00b0 - 5d 3f 3a 58 77 63 af e9-26 7f 53 b1 16 99 b3 26	 ]?:Xwc..&.S....&
	00c0 - 4f c5 5c 51 89 f5 68 28-71 16 6c b9 83 07 95 05	 O.\Q..h(q.l.....
	00d0 - 69 64 1f a3 0f fb 50 de-13 4f ed 2f 97 3c ef 1a	 id....P..O./.<..
	00e0 - 39 28 27 86 2b c4 dd aa-97 bb b0 14 42 e2 93 c4	 9('.+.......B...
	00f0 - 10 70 d0 72 24 d4 be 47-ae 27 53 eb 2b ed 4b c1	 .p.r$..G.'S.+.K.
	0100 - da 91 c6 8e c7 80 c4 62-0f 0f 02 03 01 00 01 	 .......b.......


*/
u32_t certX509::calculatePublicKeyInfoHash(const s8_t *name, chunk_t& fp)
{
	chunk_t object;
	s32_t objectID;

	asn1Parser *parser = new asn1Parser(certObjects, mDerEncoding);

	while (parser->iterate(objectID, object)) {
#if 0
		switch (objectID) {
			case X509_OBJ_TBS_CERTIFICATE:
			case X509_OBJ_VERSION:
				printf("objectID =%d,%p,%d\n",objectID,object.ptr,object.len);
				break;
			case X509_OBJ_SUBJECT_PUBLIC_KEY_INFO:
				printf("objectID2 =%d,%p,%d\n",objectID,object.ptr,object.len);
				chunk_printf(object);
				break;
			default:
				break;
		}
#else
		if (objectID == X509_OBJ_SUBJECT_PUBLIC_KEY_INFO) {
			printf("objectID2 =%d,%p,%d\n",objectID,object.ptr,object.len);
			chunk_printf(object);
			break;
		}
#endif
	}
	const EVP_MD *digest = NULL;
	unsigned char md[EVP_MAX_MD_SIZE] = {0};
	unsigned int n = 0;
	if (name == NULL) {
		digest = EVP_sha1();
	} else {
		digest = EVP_get_digestbyname(name);
	}
	EVP_Digest(object.ptr, object.len, md, &n, digest, NULL);
	fp = chunk_alloc(n); 
	memcpy(fp.ptr, md, n);

	delete parser;
	
	return STATUS_SUCCESS;
}
/*
224:d=3  hl=4 l= 271 prim: BIT STRING		 
	0000 - 00 30 82 01 0a 02 82 01-01 00 bf f2 5f 62 ea 3d	 .0.........._b.=
	0010 - 56 6e 58 b3 c8 7a 49 ca-f3 ac 61 cf a9 63 77 73	 VnX..zI...a..cws
	0020 - 4d 84 2d b3 f8 fd 6e a0-23 f7 b0 13 2e 66 26 50	 M.-...n.#....f&P
	0030 - 12 31 73 86 72 9c 6d 7c-42 7a 8d 9f 16 7b e1 38	 .1s.r.m|Bz...{.8
	0040 - e8 eb ae 2b 12 b9 59 33-ba ef 36 a3 15 c3 dd f2	 ...+..Y3..6.....
	0050 - 24 ce e4 bb 9b d5 78 13-5d 04 67 38 26 29 62 1f	 $.....x.].g8&)b.
	0060 - f9 6b 8d 45 f6 e0 02 e5-08 36 62 dc e1 81 80 5c	 .k.E.....6b....\
	0070 - 14 0b 3f 2c e9 3f 83 ae-e3 c8 61 cf f6 10 a3 9f	 ..?,.?....a.....
	0080 - 01 89 cb 3a 3c 7c b9 bf-7e 2a 09 54 4e 21 70 ef	 ...:<|..~*.TN!p.
	0090 - aa 18 fd d4 ff 20 fa 94-be 17 6d 7f ec ff 82 1f	 ..... ....m.....
	00a0 - 68 d1 71 52 04 1d 9b 46-f0 cf cf c1 e4 cf 43 de	 h.qR...F......C.
	00b0 - 5d 3f 3a 58 77 63 af e9-26 7f 53 b1 16 99 b3 26	 ]?:Xwc..&.S....&
	00c0 - 4f c5 5c 51 89 f5 68 28-71 16 6c b9 83 07 95 05	 O.\Q..h(q.l.....
	00d0 - 69 64 1f a3 0f fb 50 de-13 4f ed 2f 97 3c ef 1a	 id....P..O./.<..
	00e0 - 39 28 27 86 2b c4 dd aa-97 bb b0 14 42 e2 93 c4	 9('.+.......B...
	00f0 - 10 70 d0 72 24 d4 be 47-ae 27 53 eb 2b ed 4b c1	 .p.r$..G.'S.+.K.
	0100 - da 91 c6 8e c7 80 c4 62-0f 0f 02 03 01 00 01 	 .......b.......

X509_pubkey_digest(mpX509,fdig, md, &len); 仅包含公钥信息，不包含算法信息，BIT STRING去除00之后的数据

计算结果对应X509v3 Subject Key Identifier
X509v3 extensions:
	X509v3 Basic Constraints: critical
		CA:TRUE, pathlen:1
	X509v3 Key Usage: 
		Certificate Sign, CRL Sign
	X509v3 Subject Key Identifier: 
		5D:A7:DD:70:06:51:32:7E:E7:B6:6D:B3:B5:E5:E0:60:EA:2E:4D:EF
	X509v3 Authority Key Identifier: 
		keyid:5D:A7:DD:70:06:51:32:7E:E7:B6:6D:B3:B5:E5:E0:60:EA:2E:4D:EF
		DirName:/C=CH/O=Linux strongSwan/CN=strongSwan Root CA
		serial:00

*/
u32_t certX509::calculatePublicKeyHash(const s8_t *name, chunk_t& fp)
{
	const EVP_MD *digest = NULL;
	unsigned char md[EVP_MAX_MD_SIZE] = {0};
	unsigned int len = 0;
	if (name == NULL) {
		digest = EVP_sha1();
	} else {
		digest = EVP_get_digestbyname(name);
	}

	X509_pubkey_digest(mpX509,digest, md, &len);
	
	fp = chunk_alloc(len); 
	memcpy(fp.ptr, md, len);

	return STATUS_SUCCESS;
}

