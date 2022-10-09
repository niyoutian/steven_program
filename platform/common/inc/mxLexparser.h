#ifndef __MX_LEXPARSER_H__
#define __MX_LEXPARSER_H__
#include <sys/types.h>
#include "mxDef.h"



class mxLexparser
{
public:
	mxLexparser();
	~mxLexparser();

	u32_t getChunkFromFile(s8_t *file, chunk_t &chunk);
	u32_t fetchLine(chunk_t *src, chunk_t *line);
	bool eatWhitespace(chunk_t *src);
	u32_t extractToken(chunk_t *token, const char termination, chunk_t *src);

private:

};


#endif

