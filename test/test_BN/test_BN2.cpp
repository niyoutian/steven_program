#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <openssl/bn.h>

namespace dakuang {}

int main(int argc, char* argv[])
{
	//https://www.openssl.org/docs/man1.0.2/man3/BN_print_fp.html
/**
#include <openssl/bn.h>

int BN_bn2bin(const BIGNUM *a, unsigned char *to);
BIGNUM *BN_bin2bn(const unsigned char *s, int len, BIGNUM *ret);

char *BN_bn2hex(const BIGNUM *a);
char *BN_bn2dec(const BIGNUM *a);
int BN_hex2bn(BIGNUM **a, const char *str);
int BN_dec2bn(BIGNUM **a, const char *str);

int BN_print(BIO *fp, const BIGNUM *a);
int BN_print_fp(FILE *fp, const BIGNUM *a);

int BN_bn2mpi(const BIGNUM *a, unsigned char *to);
BIGNUM *BN_mpi2bn(unsigned char *s, int len, BIGNUM *ret);

**/
#if 0
    {
        // modulus = prime1*prime2
        BIGNUM* prime1 = BN_new();     //取自genrsa.pem里的prime1
        BIGNUM* prime2 = BN_new();    //取自genrsa.pem里的prime2
        BIGNUM* modulus  = BN_new();   //结果是genrsa.pem里的modulus
        
        BN_CTX  *ctx = BN_CTX_new();
		BN_CTX_start(ctx);
		 //sed -e 's/    //g' genrsa.txt | sed -e 's/://g'|sed -e ':a;N;$!ba;s/\n//g'
        BN_hex2bn(&prime1, "00dd33fc017f483df8e235ded80f675afc46cf06c0208efe72d8446267596e01fb6a7405c85fd5a55c94cef6d842bab904c91094a20ac0af80daf58fa4bf41060d");
        BN_hex2bn(&prime2, "00c06ddc4c84cef74ba3fa3d48626dc76bd62a27aeb6e87485945d857f42b340d39bf4c1e6daec4d57c7d3abb8e3c05bbbb8969a888c9b414f22d4f552c282b791");
        int ret = BN_mul(modulus, prime1, prime2, ctx);
        printf("BN_mul ret:%d \n", ret);
        char* pR = BN_bn2hex(modulus);
        printf("modulus=%s \n", pR);
        OPENSSL_free(pR);
		//BN_print_fp(stdout,modulus);
        BN_free(prime1);
        BN_free(prime2);
        BN_free(modulus);
		
		BN_CTX_end(ctx);
		BN_CTX_free(ctx); //配合上面的new和start
    }
#endif
	{
		// publicExponent*privateExponent  mod (prime1-1)*(prime2-1) = 1
		BIGNUM* prime1 = BN_new();	   //取自genrsa.pem里的prime1
		BIGNUM* prime2 = BN_new();	  //取自genrsa.pem里的prime2
		BIGNUM* publicExponent = BN_new();	  //取自genrsa.pem里的publicExponent		
		BIGNUM* privateExponent  = BN_new();   //结果是genrsa.pem里的privateExponent
		BIGNUM* result  = BN_new();
		BIGNUM* temp  = BN_new();
		BIGNUM* mod  = BN_new();

		
		BN_CTX	*ctx = BN_CTX_new();
		BN_CTX_start(ctx);
		 //sed -e 's/	 //g' genrsa.txt | sed -e 's/://g'|sed -e ':a;N;$!ba;s/\n//g'
		BN_hex2bn(&publicExponent, "10001");
		BN_hex2bn(&privateExponent, "154e7444042783dadcf11f7cca901ffb5b374a7516724eaccb7e16d624d01adec1dac0a44fe6efaba9aca750fe2c124c8a0f716f0b9e3e735db953aae44741bfa4e302b1f86d79604bffddd66a940ec8cbf3bf6ff9cf30f03b72961852e196ab5c59c0bf8e4dbacd12ff0462184a4c4219b02e40add4ca46cc328b9da336c681");
		int ret = BN_mul(result, publicExponent, privateExponent, ctx);
		printf("BN_mul ret:%d \n", ret);
		char* pR = BN_bn2hex(result);
		printf("result=%s \n", pR);
		OPENSSL_free(pR);

        BN_hex2bn(&prime1, "00dd33fc017f483df8e235ded80f675afc46cf06c0208efe72d8446267596e01fb6a7405c85fd5a55c94cef6d842bab904c91094a20ac0af80daf58fa4bf41060d");
        BN_hex2bn(&prime2, "00c06ddc4c84cef74ba3fa3d48626dc76bd62a27aeb6e87485945d857f42b340d39bf4c1e6daec4d57c7d3abb8e3c05bbbb8969a888c9b414f22d4f552c282b791");
		BN_sub_word(prime1, 1);
		BN_sub_word(prime2, 1);
		pR = BN_bn2hex(prime1);
		printf("prime1-1=%s \n", pR);
		OPENSSL_free(pR);

		ret = BN_mul(temp, prime1, prime2, ctx);
		printf("BN_mul ret:%d \n", ret);
		pR = BN_bn2hex(temp);
		printf("temp=%s \n", pR);
		OPENSSL_free(pR);

		ret = BN_mod(mod,result,temp,ctx);
		printf("BN_mod ret:%d \n", ret);
		pR = BN_bn2hex(mod);
		printf("mod=%s \n", pR);   //正确值 mod=01 
		OPENSSL_free(pR);

		
		BN_free(prime1);
		BN_free(prime2);
		BN_free(publicExponent);
		BN_free(privateExponent);
		BN_free(temp);
		BN_free(result);
		
		BN_CTX_end(ctx);
		BN_CTX_free(ctx); //配合上面的new和start
	}

#if 0
    {
        // 计算乘法
        BIGNUM* prime1 = BN_new();     //0xd013
        BIGNUM* prime2 = BN_new();    //0x8c27
        BIGNUM* modulus  = BN_new();   //0x71ea16e5
        
        BN_CTX  *ctx = BN_CTX_new();
		BN_CTX_start(ctx);
		 
        BN_set_word(prime1, 0xd013);
        BN_set_word(prime2, 0x8c27);
        int ret = BN_mul(modulus, prime1, prime2, ctx);
        printf("BN_mul ret:%d \n", ret);
        printf("BN_get_word:%d \n", BN_get_word(modulus));
        char* pR = BN_bn2dec(modulus);
        printf("modulus=%s \n", pR);
        OPENSSL_free(pR);
		BN_print_fp(stdout,modulus);
        BN_free(prime1);
        BN_free(prime2);
        BN_free(modulus);
		
		BN_CTX_end(ctx);
		BN_CTX_free(ctx); //配合上面的new和start
    }
#endif
    return 0;
}

