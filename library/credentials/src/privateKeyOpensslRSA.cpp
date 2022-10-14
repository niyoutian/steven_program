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


