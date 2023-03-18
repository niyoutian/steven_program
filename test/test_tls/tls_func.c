#include <stdio.h>
#include <openssl/ssl.h>
#include "tls_func.h"

SSL_CTX *gp_SslCtx = NULL;
SSL *gp_Ssl = NULL;

void setSslCtxCiphersuites(SSL_CTX *pCtx) 
{
    int ret = 0;
    STACK_OF(SSL_CIPHER)* sk = NULL;
    int i = 0;

    /* 设置算法套 
    https://www.openssl.org/docs/manmaster/man3/SSL_CTX_set_ciphersuites.html
    int SSL_CTX_set_ciphersuites(SSL_CTX *ctx, const char *str);
    https://www.openssl.org/docs/manmaster/man3/SSL_CIPHER_get_name.html
    const char *SSL_CIPHER_get_name(const SSL_CIPHER *cipher);
    */
    sk = SSL_CTX_get_ciphers(pCtx);
    for (i = 0; i < sk_SSL_CIPHER_num(sk); i++) {
        const SSL_CIPHER* c = sk_SSL_CIPHER_value(sk, i);
        unsigned long id = SSL_CIPHER_get_id(c);
        const char *name = SSL_CIPHER_get_name(c);
        printf("id:%lu name:%s\n",id, name);
    }
    /* 设置TLS1.3算法套 */
    ret = SSL_CTX_set_ciphersuites(pCtx, "TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256:TLS_AES_128_GCM_SHA256:TLS_AES_128_CCM_8_SHA256");
    printf("SSL_CTX_set_ciphersuites ret = %d\n",ret);

    sk = SSL_CTX_get_ciphers(pCtx);
    for (i = 0; i < sk_SSL_CIPHER_num(sk); i++) {
        const SSL_CIPHER* c = sk_SSL_CIPHER_value(sk, i);
        unsigned long id = SSL_CIPHER_get_id(c);
        const char *name = SSL_CIPHER_get_name(c);
        printf("id2:%lu name:%s\n",id, name);
    }

    /* 设置TLS1.2算法套 */
    /*
    int SSL_CTX_set_cipher_list(SSL_CTX *ctx, const char *str);
    https://www.openssl.org/docs/manmaster/man1/openssl-ciphers.html
    TLS_ECDHE_ECDSA_WITH_NULL_SHA           ECDHE-ECDSA-NULL-SHA
    
    */
    
    ret = SSL_CTX_set_cipher_list(pCtx, "ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-ECDSA-AES128-SHA256");
    printf("SSL_CTX_set_cipher_list ret = %d\n",ret);

    sk = SSL_CTX_get_ciphers(pCtx);
    for (i = 0; i < sk_SSL_CIPHER_num(sk); i++) {
        const SSL_CIPHER* c = sk_SSL_CIPHER_value(sk, i);
        unsigned long id = SSL_CIPHER_get_id(c);
        const char *name = SSL_CIPHER_get_name(c);
        printf("id3:%lu name:%s\n",id, name);
    }
}
/*
role 1
SSL_CTX *SSL_CTX_new(const SSL_METHOD *method);
 const SSL_METHOD *TLS_method(void);
 const SSL_METHOD *TLS_server_method(void);
 const SSL_METHOD *TLS_client_method(void);
 https://www.openssl.org/docs/man1.1.1/man3/SSLv23_client_method.html

SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,NULL) 
SSL_CTX_load_verify_locations(ctx,CACERT,NULL);
SSL_CTX_use_certificate_file(ctx, CERTF, SSL_FILETYPE_PEM) 
int SSL_CTX_use_certificate_file(SSL_CTX *ctx, const char *file, int type)；
SSL_CTX_use_PrivateKey_file(ctx, KEYF, SSL_FILETYPE_PEM)
SSL_CTX_check_private_key(ctx)

2.void SSL_CTX_set_verify(SSL_CTX *ctx,int mode,int (*callback)(int, X509_STORE_CTX *));
缺省mode是SSL_VERIFY_NONE,如果想要验证对方的话,便要将此项变成SSL_VERIFY_PEER.SSL/TLS中缺省只验证server,如果没有设置 SSL_VERIFY_PEER的话,客户端连证书都不会发过来.
3.int SSL_CTX_load_verify_locations(SSL_CTX *ctx, const char *CAfile,const char *CApath);
要验证对方的话,当然装要有CA的证书了,此函数用来便是加载CA的证书文件的.
RETURN VALUES
0  The operation failed because CAfile and CApath are NULL or the processing at one of the locations specified failed. Check the error stack to find out the reason.
1  The operation succeeded.

4.int SSL_CTX_use_certificate_file(SSL_CTX *ctx, const char *file, int type);
加载自己的证书文件.
RETURN VALUES
On success, the functions return 1. Otherwise check out the error stack to find out the reason.

5.int SSL_CTX_use_PrivateKey_file(SSL_CTX *ctx, const char *file, int type);
加载自己的私钥,以用于签名.
RETURN VALUES
On success, the functions return 1. Otherwise check out the error stack to find out the reason.

6.int SSL_CTX_check_private_key(SSL_CTX *ctx);
调用了以上两个函数后,自己检验一下证书与私钥是否配对.
https://www.cnblogs.com/yaoliang11/archive/2011/04/28/2031661.html

const char *OSSL_default_ciphersuites(void);
OSSL_default_ciphersuites() returns the default cipher string for TLSv1.3 ciphersuites.

int SSL_CTX_set_ciphersuites(SSL_CTX *ctx, const char *str);

*/
int initSslLibrary(unsigned int role)
{
    SSL_CTX *pCtx = NULL;
    int ret = 0;


    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    {
        const char * tls1_3_cipher = NULL;
        tls1_3_cipher = OSSL_default_ciphersuites();
        printf("TLS1.3 %s\n",tls1_3_cipher);
    }
    
    if (role == 1) {
        //pCtx = SSL_CTX_new(TLS_server_method());
        pCtx = SSL_CTX_new(TLSv1_2_server_method());
        //setSslCtxCiphersuites(pCtx);
        SSL_CTX_set_verify(pCtx,SSL_VERIFY_PEER,NULL);   /*验证与否*/
        ret = SSL_CTX_load_verify_locations(pCtx, CA_CERT_FILE, NULL); /*若验证,则放置CA证书*/
        if (ret != 1) {
            printf("SSL_CTX_load_verify_locations error\n");
        }
        SSL_CTX_set_default_passwd_cb_userdata(pCtx, (void*)"niyoutian");
        ret = SSL_CTX_use_certificate_file(pCtx, SERVER_CERT_FILE, SSL_FILETYPE_PEM);
        if (ret != 1) {
            printf("SSL_CTX_use_certificate_file error\n");
        }
        ret = SSL_CTX_use_PrivateKey_file(pCtx, SERVER_KEY_FILE, SSL_FILETYPE_PEM);
        if (ret != 1) {
            printf("SSL_CTX_use_PrivateKey_file error\n");
        }
        ret = SSL_CTX_check_private_key(pCtx);
        if (ret != 1) {
            printf("SSL_CTX_check_private_key error\n");
        }
    } else {
        //pCtx = SSL_CTX_new(TLS_client_method());
        pCtx = SSL_CTX_new(TLSv1_2_client_method());
        SSL_CTX_set_verify(pCtx,SSL_VERIFY_PEER,NULL);   /*验证与否*/
        ret = SSL_CTX_load_verify_locations(pCtx, "/home/nancy/cert_mgr/certs/ca_cert.pem", NULL); /*若验证,则放置CA证书*/
        if (ret != 1) {
            printf("SSL_CTX_load_verify_locations error\n");
        }
        SSL_CTX_set_default_passwd_cb_userdata(pCtx, (void*)"niyoutian");
        ret = SSL_CTX_use_certificate_file(pCtx, CLIENT_CERT_FILE, SSL_FILETYPE_PEM);
        if (ret != 1) {
            printf("SSL_CTX_use_certificate_file error\n");
        }
        ret = SSL_CTX_use_PrivateKey_file(pCtx, CLIENT_KEY_FILE, SSL_FILETYPE_PEM);
        if (ret != 1) {
            printf("SSL_CTX_use_PrivateKey_file error\n");
        }
        ret = SSL_CTX_check_private_key(pCtx);
        if (ret != 1) {
            printf("SSL_CTX_check_private_key error\n");
        }
    }
    if (pCtx == NULL) {
        return -1;
    }

    gp_SslCtx = pCtx;

    return 0;
}
/*
https://www.openssl.org/docs/man1.1.1/man3/SSL_new.html
https://www.openssl.org/docs/man1.1.1/man3/SSL_get_error.html

https://www.openssl.org/docs/man1.1.1/man3/ERR_get_error.html
#include <openssl/err.h>
unsigned long ERR_get_error(void);

https://www.openssl.org/docs/man1.1.1/man3/ERR_error_string.html
#include <openssl/err.h>
char *ERR_error_string(unsigned long e, char *buf);

*/
int bindSslAndAccept(int sockfd)
{
    int ret = 0;
    SSL *ssl = NULL;
    
    ssl = SSL_new(gp_SslCtx);
    if (ssl == NULL) {
        printf("SSL_new error %s\n",ERR_error_string(ERR_get_error(), NULL));
        return -1;
    }
    //
    ret = SSL_set_fd(ssl, sockfd);
    printf("bindSslAndAccept SSL_set_fd=%d\n",ret);
    gp_Ssl = ssl;

    ret = SSL_accept(ssl);
    printf("bindSslAndAccept SSL_accept=%d\n",ret);
    if (ret <= 0) {
        printf("SSL_accept error\n");
        return -1;
    }

    return 0;
}

int bindSslAndConnect(int sockfd)
{
    int ret = 0;
    SSL *ssl = NULL;
    
    ssl = SSL_new(gp_SslCtx);
    if (ssl == NULL) {
        printf("SSL_new error %s\n",ERR_error_string(ERR_get_error(), NULL));
        return -1;
    }
    //https://www.openssl.org/docs/man1.1.1/man3/SSL_set_fd.html
    SSL_set_fd(ssl, sockfd); 
    gp_Ssl = ssl;

    ret = SSL_connect(ssl);
    if (ret <= 0) {
        printf("SSL_connect\n");
        return -1;
    }
    printf("SSL_connect: %d\n", ret);
    
    return 0;
}

SSL* getSslFd(void) {
    return  gp_Ssl;
}