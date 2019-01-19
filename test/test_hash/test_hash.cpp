#include "stdio.h"
#include "hashSha256.h"
#include "mxLog.h"

/* echo -n "abcde" |openssl dgst -md5 
 * (stdin)= ab56b4d92b40713acc5af89985d4b786
 * echo -n "abcde" | openssl dgst -sha1
 * (stdin)= 03de6c570bfe24bfc328ccd7ca46b76eadaf4334
 * echo -n "abcde" | openssl dgst -sha224
 * (stdin)= bdd03d560993e675516ba5a50638b6531ac2ac3d5847c61916cfced6
 * echo -n "abcde" | openssl dgst -sha256
 * (stdin)= 36bbe50ed96841d10443bcb670d6554f0a34b761be67ec9c4a8ad2c0c44ca42c
 * echo -n "abcde" | openssl dgst -sha384
 * (stdin)= 4c525cbeac729eaf4b4665815bc5db0c84fe6300068a727cf74e2813521565abc0ec57a37ee4d8be89d097c0d2ad52f0
 * echo -n "abcde" | openssl dgst -sha512
 * (stdin)= 878ae65a92e86cac011a570d4c30a7eaec442b85ce8eca0c2952b5e3cc0628c2e79d889ad4d5c7c626986d452dd86374b6ffaa7cd8b67665bef2289a5c70b0a1
 */
int main(int argc, char* argv[])
{
	int i = 0;
	u8_t buf[20]={'a','b','c','d', 'e', 0};
	chunk_t chunk;
	u8_t digist[80]= {0};
	
	HashInterface *hash = new HashSha384();
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
	for(i = 0; i< hash->getHashSize(); i++)
	{
		printf("%02x ",digist[i]);
	}
	printf("\n");
	return 0;
}
