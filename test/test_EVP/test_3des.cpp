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
	
	// ��ʼ�� SSL �㷨�⺯�������� SSL ϵ�к���֮ǰ������ô˺�����
	SSL_library_init();
	
	// ���� SSL ������Ϣ
	SSL_load_error_strings();
	
	// �������� ���� �� ɢ�� ����
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


