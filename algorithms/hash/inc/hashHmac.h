#ifndef __HASH_HMAC_H__
#define __HASH_HMAC_H__
#include "mxDef.h"
#include "hashInterface.h"


/* RFC2104 HMAC: Keyed-Hashing for Message Authentication */

class HashHmac
{
public:
	HashHmac();
	~HashHmac();
	u32_t setHashType(HashType_e type);
	void setHashKey(chunk_t key);
	u32_t getHmacSize(void);
	void getHashHmac(chunk_t input, chunk_t &output);
private:
	HashInterface *mpHash;
	u32_t          mBlockLen;
  	u8_t           mIpad[128]; /* the B may 64 or 128 */
	u8_t           mOpad[128]; /* so we choose 128 , it could cover 64 */
};











#endif /* __HASH_HMAC_H__ */
