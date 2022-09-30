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
private:
};


#endif

