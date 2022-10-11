#ifndef __PRIVATE_KEY_Mgr_H__
#define __PRIVATE_KEY_Mgr_H__
using namespace std;
#include <list>
#include "mxDef.h"
#include "privateKey.h"

enum key_encoding {
	KEY_ENCODING_PEM,
	KEY_ENCODING_DER
};



class privateKeyMgr
{
public:
	privateKeyMgr();
	~privateKeyMgr();
	privateKey* loadPkcs1RsaPrivateKey(s8_t *filename,chunk_t secret);
private:
	u32_t getKeyEncoding(s8_t *filename,u32_t& encoding);
	
	list <privateKey*> mPriKeyList;
};


#endif 

