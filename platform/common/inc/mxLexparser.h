#ifndef __MX_LEXPARSER_H__
#define __MX_LEXPARSER_H__
#include <sys/types.h>
#include "mxDef.h"
#include "chunk.h"

typedef enum {
	PEM_PRE    = 0,
	PEM_MSG    = 1,
	PEM_HEADER = 2,
	PEM_BODY   = 3,
	PEM_POST   = 4,
	PEM_ABORT  = 5
} PEM_State_e;


class mxLexparser
{
public:
	mxLexparser();
	~mxLexparser();

	u32_t getChunkFromFile(s8_t *file, chunk_t &chunk);
	u32_t fetchLine(chunk_t *src, chunk_t *line);
	bool eatWhitespace(chunk_t *src);
	u32_t extractToken(chunk_t *src, const char termination, chunk_t *token);
	u32_t extractFilenameSecret(chunk_t *src, chunk_t *filename, chunk_t *secret);
	u32_t convertPEM2BIN(chunk_t src, chunk_t &chunk);
private:

};


#endif

