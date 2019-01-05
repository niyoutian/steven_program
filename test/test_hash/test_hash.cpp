#include "stdio.h"
#include "hashMd5.h"
#include "mxLog.h"


int main(int argc, char* argv[])
{
	int i = 0;
	chunk_t chunk;
	u8_t digist[16]= {0};
	
	HashInterface *hash = new HashMd5();
	if (hash == NULL)
	{
		printf("new HashMd5 error\n");
	}
	printf("hash type: %d\n",hash->getHashType());
	printf("hash size: %d\n",hash->getHashSize());
	chunk.ptr = "a";
	chunk.len = 1;

	hash->calcHash(chunk, digist);
	printf("md5hash: ");
	for(i = 0; i< 16; i++)
	{
		printf("%02x ",digist[i]);
	}
	
	return 0;
}
