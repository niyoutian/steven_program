#include "mxLexparser.h"
#include "privateKeyGmpRSA.h"





privateKeyGmpRSA::privateKeyGmpRSA() 
{

}

privateKeyGmpRSA::~privateKeyGmpRSA() 
{

}

u32_t privateKeyGmpRSA::loadPriKeyFromPEM(s8_t *filename, chunk_t secret)
{
	printf("privateKeyGmpRSA::loadPriKeyFromPEM\n");
	
	mxLexparser *parser = new mxLexparser();
	chunk_t data = {NULL, 0};
	chunk_t bin = {NULL, 0};

	parser->getChunkFromFile(filename,data);

	parser->convertPEM2BIN(data, bin);
	
	return 0;
}

u32_t privateKeyGmpRSA::loadPriKeyFromDER(s8_t *filename)
{
	printf("privateKeyGmpRSA::loadPriKeyFromDER\n");
	return 0;
}