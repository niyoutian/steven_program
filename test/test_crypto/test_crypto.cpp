#include "stdio.h"
#include "symmCryptoBase.h"
#include "symmCryptoSsl.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/aes.h>


/*
https://blog.csdn.net/swj9099/article/details/89668980

*/

mxSymmEncBase *g_enc = NULL;


int encrypt(char *input_file, char *output_file, unsigned char *key,unsigned char *iv) 
{
	FILE *inFile = NULL;
	FILE *outFile = NULL;
	int count = 0;
	unsigned char inBuffer[AES_BLOCK_SIZE] = {0};
	unsigned char outBuffer[AES_BLOCK_SIZE] = {0};
	int i = 0;
	char padding = 0;
	int do_enc = 1; 
	chunk_t data = {inBuffer,(u32_t)AES_BLOCK_SIZE};
	chunk_t aiv = {iv,(u32_t)AES_BLOCK_SIZE};
	chunk_t dst = {outBuffer,(u32_t)AES_BLOCK_SIZE};

	printf("enter encrypt\n");

	inFile = fopen(input_file,"rb");

	if (inFile == NULL) {
		return -1;
	}
	
	outFile = fopen(output_file, "wb");
	if (outFile == NULL) {
		fclose(inFile);
		return -2;
	}
	while (do_enc) {
		count = fread(inBuffer, 1, AES_BLOCK_SIZE, inFile);
		printf("count %d\n",count);
		if (count < AES_BLOCK_SIZE) {
			do_enc = 0;
			/* pkcs7 */
			padding = AES_BLOCK_SIZE - (count % AES_BLOCK_SIZE);
			for (i = count % 16; i < 16; i++) {
				inBuffer[i] = padding;
			}
		}
		//encrypt_function(inBuffer, outBuffer,AES_BLOCK_SIZE, key, iv);
		g_enc->setKey(key,AES_BLOCK_SIZE);
		g_enc->encrypt(data,aiv,&dst);   //aiv是入参，不会更新aiv
		/*  AES_cbc_encrypt(plaint_str, encrypt_str, len, &aes, iv, AES_ENCRYPT); 
		会更新iv 到加密输出的密文  openssl用的是这一种*/
		memcpy(aiv.ptr, dst.ptr, AES_BLOCK_SIZE);
		fwrite(outBuffer, 1, AES_BLOCK_SIZE, outFile);
		memset(inBuffer,0,AES_BLOCK_SIZE);
	}

	fclose(inFile);
	fclose(outFile);
	
	return 0;
}

int decrypt(char *input_file, char *output_file, unsigned char *key,unsigned char *iv) 
{
#if 0
	FILE *inFile = NULL;
	FILE *outFile = NULL;
	int count = 0;
	char inBuffer[AES_BLOCK_SIZE];
	char outBuffer[AES_BLOCK_SIZE];
	int total = 0;
	int fileLen = 0;
	int do_dec = 1; 


	printf("enter decrypt\n");

	inFile = fopen(input_file,"rb");

	if (inFile == NULL) {
		return -1;
	}

	fileLen = filesize(inFile);
	printf("fileLen %d\n",fileLen);
	if (fileLen % AES_BLOCK_SIZE) {
		fclose(inFile);
		return -3;
	}
	outFile = fopen(output_file, "wb");
	if (outFile == NULL) {
		fclose(inFile);
		return -2;
	}
	printf("decrypt %p %p\n",inFile,outFile);
	while (do_dec) {
		count = fread(inBuffer, 1, AES_BLOCK_SIZE, inFile);

		printf("count %d %d\n",count, total);
		decrypt_function(inBuffer, outBuffer,AES_BLOCK_SIZE, key, iv);
		total += count;
		if (total >= fileLen) {
			do_dec = 0;
			if (outBuffer[AES_BLOCK_SIZE-1] != AES_BLOCK_SIZE) {
				fwrite(outBuffer, 1, AES_BLOCK_SIZE-outBuffer[AES_BLOCK_SIZE-1], outFile);
			}
		} else {
			fwrite(outBuffer, 1, AES_BLOCK_SIZE, outFile);
		}
	}

	fclose(inFile);
	fclose(outFile);
#endif
	return 0;
}


int main(int argc, char* argv[])
{
	unsigned char key[16]={0x8c,0xc7,0x2b,0x05,0x70,0x5d,0x5c,0x46,0xf4,0x12,0xaf,0x8c,0xbe,0xd5,0x5a,0xad};
	unsigned char iv[16]= {0x66,0x7b,0x02,0xa8,0x5c,0x61,0xc7,0x86,0xde,0xf4,0x52,0x1b,0x06,0x02,0x65,0xe8};
    unsigned char iv2[16]={0x66,0x7b,0x02,0xa8,0x5c,0x61,0xc7,0x86,0xde,0xf4,0x52,0x1b,0x06,0x02,0x65,0xe8};

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

	ret = g_enc->setCryptoType(ENCR_AES_CBC,16);
	printf("ret=%u\n",ret);
	printf("block=%u\n",g_enc->getBlockSize());

	printf("hello world\n");
	encrypt("/home/steven/study2/stock/steven_program/test/test_crypto/plain.txt", "/home/steven/study2/stock/steven_program/test/test_crypto/plain5.txt",key,iv);
	printf("after encrypt\n");
	decrypt("/home/steven/study2/stock/steven_program/test/test_crypto/encrypt.txt", "/home/steven/study2/stock/steven_program/test/test_crypto/encrypt_decrypt2.txt",key,iv2);

	return 0;
}


