#include "stdio.h"
#include "symmCryptoBase.h"
#include "symmCryptoSsl.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/aes.h>





mxSymmEncBase *g_enc = NULL;


int main(int argc, char* argv[])
{
	u32_t ret = 0;
	
	// 初始化 SSL 算法库函数，调用 SSL 系列函数之前必须调用此函数！
	SSL_library_init();
	
	// 加载 SSL 错误消息
	SSL_load_error_strings();
	
	// 加载所有 加密 和 散列 函数
	OpenSSL_add_all_algorithms();

	g_enc = new SymmCryptoSSl();
	if (g_enc == NULL)
	{
		printf("new enc error\n");
	}

	ret = g_enc->setCryptoType(ENCR_3DES,24);
	printf("ret=%u\n",ret);
	printf("block=%u\n",g_enc->getBlockSize());

	printf("hello world\n");

	printf("3des\n");
	return 0;
}


