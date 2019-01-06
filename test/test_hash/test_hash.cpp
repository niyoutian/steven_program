#include "stdio.h"
#include "hashSha256.h"
#include "mxLog.h"

/* echo -n "abcde" |openssl dgst -md5 
 * (stdin)= ab56b4d92b40713acc5af89985d4b786
 * echo -n "abcde" | openssl dgst -sha1
 * (stdin)= 03de6c570bfe24bfc328ccd7ca46b76eadaf4334
 * echo -n "abcde" | openssl dgst -sha256
 * (stdin)= 36bbe50ed96841d10443bcb670d6554f0a34b761be67ec9c4a8ad2c0c44ca42c
 */
int main(int argc, char* argv[])
{
	int i = 0;
	u8_t buf[20]={'a','b','c','d', 'e', 0};
	chunk_t chunk;
	u8_t digist[60]= {0};
	
	HashInterface *hash = new HashSha256();
	if (hash == NULL)
	{
		printf("new HashSha1 error\n");
	}
	printf("hash type: %d\n",hash->getHashType());
	printf("hash size: %d\n",hash->getHashSize());
	chunk.ptr = buf;
	chunk.len = 5;

	hash->calcHash(chunk, digist);
	printf("sha1hash: ");
	for(i = 0; i< 32; i++)
	{
		printf("%02x ",digist[i]);
	}
	printf("\n");
	return 0;
}
