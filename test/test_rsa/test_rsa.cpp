#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/objects.h>



namespace dakuang {}

void printHex(const unsigned char* pBuf, int nLen)
{
    for (int i = 0; i < nLen; ++i)
    {
        printf("%02x", pBuf[i]);
    }
    printf("\n");
}



int main(int argc, char* argv[])
{
#if 0
/*
下面这个例子演示了RSA密钥对的生成、公私钥的复制、公钥加密和私钥解密的用法。

*/
    ERR_load_RSA_strings();

    RSA* pRSA = RSA_generate_key(512, RSA_3, NULL, NULL);
    RSA_print_fp(stdout, pRSA , 0);
    RSA_free(pRSA);

    pRSA = RSA_new();
    BIGNUM* pBNe = BN_new();
    BN_set_word(pBNe, RSA_3);
    int ret = RSA_generate_key_ex(pRSA, 512, pBNe, NULL);
    printf("RSA_generate_key_ex() ret:%d \n", ret);
    BN_free(pBNe);
    RSA_print_fp(stdout, pRSA , 0);

    RSA* pRSApub = RSAPublicKey_dup(pRSA);
    printf("copy pub key:%p \n", pRSApub);
    RSA_print_fp(stdout, pRSApub , 0);
    RSA_free(pRSApub);
    RSA* pRSApri = RSAPrivateKey_dup(pRSA);
    printf("copy pri key:%p \n", pRSApri);
    RSA_print_fp(stdout, pRSApri , 0);
    RSA_free(pRSApri);

    int bits = RSA_bits(pRSA);
    printf("bits:%d \n", bits);
    int bytes = RSA_size(pRSA);
    printf("bytes:%d \n", bytes);

    ret = RSA_check_key(pRSA);
    printf("RSA_check_key() ret:%d \n", ret);

    char sText[] = "1234567890";

    unsigned char* pCipher = (unsigned char*)malloc(bytes);
	//密文的长度与私钥长度一致, 和RSA的modulus，privateExponent长度也是一致的
    ret = RSA_public_encrypt(strlen(sText), (const unsigned char*)sText, pCipher, pRSA, RSA_PKCS1_PADDING);
    printf("RSA_public_encrypt() ret:%d \n", ret);
    if (ret <= 0)
    {
        unsigned long err = ERR_get_error();
        printf("err:[%ld] \n", err);
        char sBuf[128] = {0};
        char* pErrStr = ERR_error_string(err, sBuf);
        printf("errstr:[%s] \n", pErrStr);
    }

    unsigned char* pText = (unsigned char*)malloc(bytes);
    ret = RSA_private_decrypt(ret, (const unsigned char*)pCipher, pText, pRSA, RSA_PKCS1_PADDING);
    printf("RSA_private_decrypt() ret:%d \n", ret);
    if (ret > 0)
    {
        printf("text:[%s] \n", pText);
    }

    free(pCipher);
    free(pText);

    RSA_free(pRSA);
#else
#if 0
/*
下面这个例子演示了公私钥的分开保存、读取，以及使用公私钥加解密

*/
	ERR_load_RSA_strings();
	int ret = 0;

/*
	RSA* pRSA = RSA_new();
	BIGNUM* pBNe = BN_new();
	BN_set_word(pBNe, RSA_3);
	ret = RSA_generate_key_ex(pRSA, 512, pBNe, NULL);
	printf("RSA_generate_key_ex() ret:%d \n", ret);
	BN_free(pBNe);
	RSA_free(pRSA);

	BIO* pBIOpri = BIO_new_file("rsa.key","w");
	BIO* pBIOpub = BIO_new_file("rsa.pub","w");
	ret = i2d_RSAPrivateKey_bio(pBIOpri, pRSA);
	printf("i2d_RSAPrivateKey_bio() ret:%d \n", ret);
	ret = i2d_RSAPublicKey_bio(pBIOpub, pRSA);
	printf("i2d_RSAPublicKey_bio() ret:%d \n", ret);
	BIO_free(pBIOpri);
	BIO_free(pBIOpub);*/

	RSA* pRSApri = RSA_new();
	RSA* pRSApub = RSA_new();
	BIO* pBIOpri2 = BIO_new_file("rsa.key","r");
	BIO* pBIOpub2 = BIO_new_file("rsa.pub","r");
	d2i_RSAPrivateKey_bio(pBIOpri2, &pRSApri);
	d2i_RSAPublicKey_bio(pBIOpub2, &pRSApub);//从bp抽象IO中读取DER编码，并转换为rsa结构公钥。
	BIO_free(pBIOpri2);
	BIO_free(pBIOpub2);

	int bytes = RSA_size(pRSApri);
	printf("copy' private key size:%d \n", bytes);
	int bytes2 = RSA_size(pRSApub);
	printf("copy' public key size:%d \n", bytes2);

	char sText[] = "1234567890";

	unsigned char* pCipher = (unsigned char*)malloc(bytes);
	ret = RSA_public_encrypt(strlen(sText), (const unsigned char*)sText, pCipher, pRSApub, RSA_PKCS1_PADDING);
	printf("RSA_public_encrypt() ret:%d \n", ret);
	if (ret <= 0)
	{
		unsigned long err = ERR_get_error();
		printf("err:[%ld] \n", err);
		char sBuf[128] = {0};
		char* pErrStr = ERR_error_string(err, sBuf);
		printf("errstr:[%s] \n", pErrStr);
	}

	unsigned char* pText = (unsigned char*)malloc(bytes);
	ret = RSA_private_decrypt(ret, (const unsigned char*)pCipher, pText, pRSApri, RSA_PKCS1_PADDING);
	printf("RSA_private_decrypt() ret:%d \n", ret);
	if (ret > 0)
	{
		printf("text:[%s] \n", pText);
	}
	free(pCipher);
	free(pText);

	RSA_free(pRSApri);
	RSA_free(pRSApub);
	
#else

/*
下面这个例子演示了签名的生成和验证操作。

*/

#endif

#if 1
	ERR_load_RSA_strings();
	int ret = 0;


	RSA* pRSA = RSA_new();
#if 0
	BIGNUM* pBNe = BN_new();
	BN_set_word(pBNe, RSA_3);
	ret = RSA_generate_key_ex(pRSA, 512, pBNe, NULL);
	printf("ret:%d \n", ret);
	BN_free(pBNe);
#else

	BIO* pBIOpri2 = BIO_new_file("rsa.key","r");
	d2i_RSAPrivateKey_bio(pBIOpri2, &pRSA);
	BIO_free(pBIOpri2);

#endif

	char sData[] = "1234567890";
	char sSign[512] = {0};
	unsigned int nSignLen = 512;
	ret = RSA_sign(NID_sha1, (unsigned char *)sData, 10, (unsigned char *)sSign, &nSignLen, pRSA);
	printf("RSA_sign() ret:%d \n", ret);
	printf("sign %d \n", nSignLen);
	printHex((const unsigned char*)sSign, nSignLen);


	ret = RSA_verify(NID_sha1, (unsigned char *)sData, 10, (unsigned char *)sSign, nSignLen, pRSA);
	printf("RSA_verify() ret:%d \n", ret);

	RSA_free(pRSA);
#endif

#endif
    return 0;
}
