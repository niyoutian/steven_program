#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <openssl/bn.h>

namespace dakuang {}

int main(int argc, char* argv[])
{
    {
        // 计算加法
        BIGNUM* pBNa = BN_new();
        BIGNUM* pBNb = BN_new();
        BIGNUM* pBNr = BN_new();
        BN_set_word(pBNa, 1);
        BN_set_word(pBNb, 2);
        int ret = BN_add(pBNr, pBNa, pBNb);
        printf("BN_add ret:%d \n", ret);
        printf("BN_get_word:%d \n", BN_get_word(pBNr));
        char* pR = BN_bn2dec(pBNr);
        printf("1+2=%s \n", pR);
        OPENSSL_free(pR);
        BN_free(pBNa);
        BN_free(pBNb);
        BN_free(pBNr);
    }

    {
        // 这里模拟了知道公钥的情况下，执行加密计算

        // 计算次方
        BN_CTX* pBNctx = BN_CTX_new();
        BIGNUM* pBNr = BN_new();
        BIGNUM* pBNa = BN_new();
        BIGNUM* pBNp = BN_new();
        BN_set_word(pBNa, 225);
        BN_set_word(pBNp, 29);
        int ret = BN_exp(pBNr, pBNa, pBNp, pBNctx);
        printf("BN_exp ret:%d \n", ret);
        char* pR = BN_bn2dec(pBNr);
        printf("225^29=%s \n", pR);
        OPENSSL_free(pR);

        // 计算除法
        BN_CTX* pBNctx2 = BN_CTX_new();
        BIGNUM* pBNdiv = BN_new();
        BIGNUM* pBNrem = BN_new();
        BIGNUM* pBNd = BN_new();
        BN_set_word(pBNd, 323);
        ret = BN_div(pBNdiv, pBNrem, pBNr, pBNd, pBNctx2);
        printf("BN_div ret:%d \n", ret);
        pR = BN_bn2dec(pBNrem);
        printf("(225^29)%323=%s \n", pR);
        OPENSSL_free(pR);
    
        BN_free(pBNr);
        BN_free(pBNa);
        BN_free(pBNp);
        BN_free(pBNdiv);
        BN_free(pBNrem);
        BN_free(pBNd);
        BN_CTX_free(pBNctx);
        BN_CTX_free(pBNctx2);
    }

    {
        // 生成强随机数
        BIGNUM* pBNr = BN_new();
        int ret = BN_rand(pBNr, 8, BN_RAND_TOP_ANY, BN_RAND_BOTTOM_ANY);
        printf("BN_rand ret:%d \n", ret);
        char* pR = BN_bn2dec(pBNr);
        printf("rand %s \n", pR);
        OPENSSL_free(pR);
        BN_free(pBNr);
    }

    {
        // 生成质数
        BIGNUM* pBNr = BN_new();
        int ret = BN_generate_prime_ex(pBNr, 16, 1, NULL, NULL, NULL);
        printf("BN_generate_prime_ex ret:%d \n", ret);
        char* pR = BN_bn2dec(pBNr);
        printf("prime %s \n", pR);
        OPENSSL_free(pR);
        BN_free(pBNr);
    }

    {
        // 计算最大公约数
        BN_CTX* pBNctx = BN_CTX_new();
        BIGNUM* pBNr = BN_new();
        BIGNUM* pBNa = BN_new();
        BIGNUM* pBNb = BN_new();
        BN_set_word(pBNa, 10);
        BN_set_word(pBNb, 2);
        int ret = BN_gcd(pBNr, pBNa, pBNb, pBNctx);
        printf("BN_gcd ret:%d \n", ret);
        char* pR = BN_bn2dec(pBNr);
        printf("10 and 2 gcd %s \n", pR);
        OPENSSL_free(pR);
        BN_free(pBNr);
        BN_free(pBNa);
        BN_free(pBNb);
        BN_CTX_free(pBNctx);
    }

    return 0;
}

