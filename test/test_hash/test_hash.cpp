#include "stdio.h"
#include "hashMd5.h"
#include "mxLog.h"


int main(int argc, char* argv[])
{
	HashInterface *hash = new HashMd5();
	if (hash == NULL)
	{
		printf("new HashMd5 error\n");
	}
	printf("hash type: %d\n",hash->getHashType());
	printf("hash size: %d\n",hash->getHashSize());
	
	return 0;
}
