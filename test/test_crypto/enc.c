#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/aes.h>
//#include "decrypt.h"


//判断文件大小
static long filesize(FILE *stream)
{
	long curpos, length;
	curpos = ftell(stream);
	fseek(stream, 0L, SEEK_END);
	length = ftell(stream);
	fseek(stream, curpos, SEEK_SET);
	return length;
}

static int decrypt_function(unsigned char *encrypt_str, unsigned char *decrypt_str, int len, unsigned char *key, unsigned char *iv)
{
	AES_KEY aes;

	memset(&aes, 0, sizeof(aes));
	
	if (AES_set_decrypt_key(key, 128, &aes) < 0) {
        return -1;
    }
	
    // decrypt
    AES_cbc_encrypt(encrypt_str, decrypt_str, len, &aes, iv, AES_DECRYPT);
	
	return 0;
}
#if 0
int decrypt(char *input_file, char *output_file, unsigned char *key,unsigned char *iv) 
{
	FILE *inFile = NULL;
	FILE *outFile = NULL;
	int count = 0;
	char inBuffer[AES_BLOCK_SIZE];
	char outBuffer[AES_BLOCK_SIZE];


	printf("enter decrypt %s\n",input_file);

	inFile = fopen(input_file,"rb");
	printf("inFile %p\n",inFile);
	if (inFile == NULL) {
		return -1;
	}
	
	outFile = fopen(output_file, "wb");
	if (outFile == NULL) {
		fclose(inFile);
		return -2;
	}
	printf("decrypt %p %p\n",inFile,outFile);
	while ((count = fread(inBuffer, 1, AES_BLOCK_SIZE, inFile)) > 0) {
		printf("count %d\n",count);
		decrypt_function(inBuffer, outBuffer,AES_BLOCK_SIZE, key, iv);
		fwrite(outBuffer, 1, AES_BLOCK_SIZE, outFile);
	}

	fclose(inFile);
	fclose(outFile);
	
	return 0;
}
#else
int decrypt(char *input_file, char *output_file, unsigned char *key,unsigned char *iv) 
{
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
	
	return 0;
}
#endif
static int encrypt_function(unsigned char *plaint_str, unsigned char *encrypt_str, int len, unsigned char *key, unsigned char *iv)
{
	AES_KEY aes;

	memset(&aes, 0, sizeof(aes));
	
	if (AES_set_encrypt_key(key, 128, &aes) < 0) {
        return -1;
    }
	
    AES_cbc_encrypt(plaint_str, encrypt_str, len, &aes, iv, AES_ENCRYPT);
	
	return 0;
}


int encrypt(char *input_file, char *output_file, unsigned char *key,unsigned char *iv) 
{
	FILE *inFile = NULL;
	FILE *outFile = NULL;
	int count = 0;
	char inBuffer[AES_BLOCK_SIZE] = {0};
	char outBuffer[AES_BLOCK_SIZE] = {0};
	int i = 0;
	char padding = 0;
	int do_enc = 1; 


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
		encrypt_function(inBuffer, outBuffer,AES_BLOCK_SIZE, key, iv);
		fwrite(outBuffer, 1, AES_BLOCK_SIZE, outFile);
		memset(inBuffer,0,AES_BLOCK_SIZE);
	}

	fclose(inFile);
	fclose(outFile);
	
	return 0;
}

int main(int argc, char *argv[])
{
	unsigned char key[16]={0x8c,0xc7,0x2b,0x05,0x70,0x5d,0x5c,0x46,0xf4,0x12,0xaf,0x8c,0xbe,0xd5,0x5a,0xad};
	unsigned char iv[16]= {0x66,0x7b,0x02,0xa8,0x5c,0x61,0xc7,0x86,0xde,0xf4,0x52,0x1b,0x06,0x02,0x65,0xe8};
    unsigned char iv2[16]={0x66,0x7b,0x02,0xa8,0x5c,0x61,0xc7,0x86,0xde,0xf4,0x52,0x1b,0x06,0x02,0x65,0xe8};

	printf("hello world\n");
	encrypt("/home/steven/study2/enc/plain.txt", "/home/steven/study2/enc/plain2.txt",key,iv);
	printf("after encrypt\n");
	decrypt("/home/steven/study2/enc/encrypt.txt", "/home/steven/study2/enc/encrypt_decrypt2.txt",key,iv2);
	return 0;
}


