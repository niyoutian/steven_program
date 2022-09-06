#include <openssl/bio.h>

#include <openssl/pem.h>
#include "certX509.h"



certX509::certX509()
{
	
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

