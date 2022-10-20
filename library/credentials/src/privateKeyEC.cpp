#include "privateKeyEC.h"


privateKeyEC::privateKeyEC() 
{
	mpPriKey = NULL;
}

privateKeyEC::~privateKeyEC() 
{
	if (mpPriKey) {
		EVP_PKEY_free(mpPriKey);
	}
}

u32_t privateKeyEC::getKeyType(void)
{
	return KEY_ECDSA;
}

u32_t privateKeyEC::loadPriKeyFromPEM(s8_t *filename, chunk_t secret)
{
	printf("privateKeyEC::loadPriKeyFromPEM\n");
	BIO* pBIOpri = BIO_new_file(filename,"r");
	
    mpPriKey = PEM_read_bio_PrivateKey(pBIOpri, NULL,privateKeyEC::pemPasswordCb, &secret);

	printf("mpPriKey:%p\n",mpPriKey);

	BIO_free(pBIOpri);
	if (mpPriKey == NULL) {
		printf("unable to load %s\n",filename);
		return STATUS_FAILED;
	}

	return STATUS_SUCCESS;

}

u32_t privateKeyEC::loadPriKeyFromDER(s8_t *filename)
{
	printf("privateKeyEC::loadPriKeyFromDER\n");

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
s32_t privateKeyEC::pemPasswordCb (s8_t *buf, s32_t size, s32_t rwflag, void *userdata)
{
	chunk_t *pSecret = (chunk_t *)userdata;
	if (pSecret->len != 0) {
		memcpy(buf, pSecret->ptr, pSecret->len);
	}

	return pSecret->len;
}

u32_t privateKeyEC::calcSignature(u32_t sign_type, chunk_t data, chunk_t *signature)
{
	return STATUS_SUCCESS;
}

