
#include "privateKeyOpensslRSA.h"



privateKeyOpensslRSA::privateKeyOpensslRSA() 
{
	mpPriKey = NULL;
}

privateKeyOpensslRSA::~privateKeyOpensslRSA() 
{
	if (mpPriKey) {
		EVP_PKEY_free(mpPriKey);
	}
}

/*
如果pem格式的文件没有密码，不会进入回调函数pemPasswordCb
也可以用PEM_read_bio_PrivateKey(pBIOpri, NULL, NULL, NULL);
*/
u32_t privateKeyOpensslRSA::loadPriKeyFromPEM(s8_t *filename, chunk_t secret)
{
	printf("privateKeyOpensslRSA::loadPriKeyFromPEM\n");
	BIO* pBIOpri = BIO_new_file(filename,"r");
	
    mpPriKey = PEM_read_bio_PrivateKey(pBIOpri, NULL,privateKeyOpensslRSA::pemPasswordCb, &secret);

	printf("mpPriKey:%p\n",mpPriKey);

	BIO_free(pBIOpri);
	if (mpPriKey == NULL) {
		printf("unable to load %s\n",filename);
		return STATUS_FAILED;
	}

	return STATUS_SUCCESS;
}

u32_t privateKeyOpensslRSA::loadPriKeyFromDER(s8_t *filename)
{
	printf("privateKeyOpensslRSA::loadPriKeyFromDER\n");

	BIO* pBIOpri = BIO_new_file(filename,"r");
	mpPriKey = d2i_PrivateKey_bio(pBIOpri, NULL);

	BIO_free(pBIOpri);
	
	if (mpPriKey == NULL) {
		printf("unable to load %s\n",filename);
		return STATUS_FAILED;
	}
	return STATUS_SUCCESS;
}

/*
https://www.openssl.org/docs/man1.0.2/man3/PEM_read_PrivateKey.html
size=1024

*/
s32_t privateKeyOpensslRSA::pemPasswordCb (s8_t *buf, s32_t size, s32_t rwflag, void *userdata)
{
	chunk_t *pSecret = (chunk_t *)userdata;
	if (pSecret->len != 0) {
		memcpy(buf, pSecret->ptr, pSecret->len);
	}

	return pSecret->len;
}

u32_t privateKeyOpensslRSA::calcSignature(u32_t sign_type, chunk_t data, chunk_t *signature)
{
	string algname;
	u32_t ret = 0;
	
	switch (sign_type) {
		case SIGN_RSA_EMSA_PKCS1_NULL:
			algname.clear();
			ret = buildEmsaPkcs1Signature(algname, data, signature);
			break;
		case SIGN_RSA_EMSA_PKCS1_SHA2_224:
			algname.assign("SHA224");
			ret = buildEmsaPkcs1Signature(algname, data, signature);
			break;
		case SIGN_RSA_EMSA_PKCS1_SHA2_256:
			algname.assign("SHA256");
			break;
		case SIGN_RSA_EMSA_PKCS1_SHA2_384:
			algname.assign("SHA384");
			break;
		case SIGN_RSA_EMSA_PKCS1_SHA2_512:
			algname.assign("SHA512");
			break;
		case SIGN_RSA_EMSA_PKCS1_SHA1:
			algname.assign("SHA1");
			break;
		case SIGN_RSA_EMSA_PKCS1_MD5:
			algname.assign("MD5");
			break;
		case SIGN_RSA_EMSA_PSS:
			break;
		default:
			break;

	}
	return STATUS_SUCCESS;
}

u32_t privateKeyOpensslRSA::buildEmsaPkcs1Signature(const string algname, chunk_t data, chunk_t *signature)
{
	EVP_MD_CTX *mdctx = NULL;		//摘要算法上下文变量
	const EVP_MD *pMd = NULL;

	//以下是计算签名代码
	mdctx = EVP_MD_CTX_new();
	printf("mdctx=%p\n",mdctx);

	pMd = EVP_get_digestbyname(algname.c_str());
	
	if(!EVP_SignInit_ex(mdctx, pMd, NULL))//签名初始化;设置摘要算法;本例为SHA1
	{
		printf("err\n");
		return STATUS_FAILED;
	}

	return STATUS_SUCCESS;
}