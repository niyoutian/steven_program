#include <stdio.h>
#include "evpHash.h"


/* echo -n "abcde" | openssl dgst -md4
 * (stdin)= 9803f4a34e8eb14f96adba49064a0c41
 * echo -n "abcde" |openssl dgst -md5 
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
 * echo -n "abcde" | openssl dgst -sha256 -hmac "123456"
 * (stdin)= 99a5016fdb72a79f5ee487ef22536ba1e8f7aa50178758ce3bce4e2f50807b09
 * echo -n "abcde" | openssl dgst -sm3
 * (stdin)= afe4ccac5ab7d52bcae36373676215368baf52d3905e1fecbe369cc120e97628
 * echo -n "abcde" | openssl dgst -sha3-224
 * (stdin)= 6acfaab70afd8439cea3616b41088bd81c939b272548f6409cf30e57
 * echo -n "abcde" | openssl dgst -sha3-256
 * (stdin)= d716ec61e18904a8f58679b71cb065d4d5db72e0e0c3f155a4feff7add0e58eb
 * echo -n "abcde" | openssl dgst -sha3-384
 * (stdin)= 348494236b82edda7602c78ba67fc3838e427c63c23e2c9d9aa5ea6354218a3c2ca564679acabf3ac6bf5378047691c4
 * echo -n "abcde" | openssl dgst -sha3-512
 * (stdin)= 1d7c3aa6ee17da5f4aeb78be968aa38476dbee54842e1ae2856f4c9a5cd04d45dc75c2902182b07c130ed582d476995b502b8777ccf69f60574471600386639b
 */

int main()
{ 
	u32_t size = 0;
	u8_t buf[20]={'a','b','c','d', 'e', 0};
	chunk_t chunk;
	u8_t digist[80]= {0};
	
	evpHash *hash = new evpHash();

	printf("EVP hash\n");

	hash->setHashType(HASH_MD4);
	size = hash->getHashSize();

	printf("hash:%s size:%d\n",hash->getHashName(),size);

	chunk.ptr = buf;
	chunk.len = 5;

	hash->initHash();
	hash->calcHash(chunk, digist);
	printf("sha1hash: ");
	for(int i = 0; i< hash->getHashSize(); i++)
	{
		printf("%02x ",digist[i]);
	}
	printf("\n");


	delete hash;
	return 0;
}

