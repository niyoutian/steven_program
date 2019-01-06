#include "stdio.h"
#include "hashMd5.h"
#include "mxLog.h"


int main(int argc, char* argv[])
{
	int i = 0;
	u8_t buf[20]={'a','b','c','d', 'e', 0};
	chunk_t chunk;
	u8_t digist[16]= {0};
	
	HashInterface *hash = new HashMd5();
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
	for(i = 0; i< 16; i++)
	{
		printf("%02x ",digist[i]);
	}
	printf("\n");
	return 0;
}
