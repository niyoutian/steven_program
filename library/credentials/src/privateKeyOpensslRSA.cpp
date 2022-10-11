#include "privateKeyOpensslRSA.h"



privateKeyOpensslRSA::privateKeyOpensslRSA() 
{

}

privateKeyOpensslRSA::~privateKeyOpensslRSA() 
{

}

u32_t privateKeyOpensslRSA::loadPriKeyFromPEM(s8_t *filename, chunk_t secret)
{
	printf("privateKeyOpensslRSA::loadPriKeyFromPEM\n");
	
	
	return 0;
}

u32_t privateKeyOpensslRSA::loadPriKeyFromDER(s8_t *filename)
{
	printf("privateKeyOpensslRSA::loadPriKeyFromDER\n");
	return 0;
}


