#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "mxDef.h"
#include "mxStatus.h"
#include "mxLog.h"
#include "mxLexparser.h"


mxLexparser::mxLexparser()
{

}

mxLexparser::~mxLexparser()
{

}

u32_t mxLexparser::getChunkFromFile(s8_t *file, chunk_t &chunk)
{
	s32_t fd = 0;
	struct stat sb;
	u32_t ret = 0;
	
	fd = open(file, O_RDONLY);
	if (fd == -1) {
		return STATUS_FAILED;
	}

	if (fstat(fd, &sb) == -1){
		return STATUS_FAILED;
	}

	s8_t *buf = NULL;
	s32_t len = 0, total = 0;
	buf = (s8_t *)malloc(sb.st_size);
	if (buf== NULL) {
		return STATUS_FAILED;
	}
	while (true) {
		len = read(fd, buf + total, sb.st_size - total);
		if (len < 0) {
			close(fd);
			free(buf);
			return STATUS_FAILED;
		}
		if (len == 0) {
			break;
		}
		total += len;
	}

	close(fd);

	chunk.ptr = (u8_t *)buf;
	chunk.len = sb.st_size;
	
	return STATUS_SUCCESS;
}

u32_t mxLexparser::fetchLine(chunk_t *src, chunk_t *line)
{
	if (src->len == 0) {/* end of src reached */
		return STATUS_FAILED;
	}
	
	if (extractToken(line, '\n', src) == STATUS_SUCCESS)
	{
		if (line->len > 0 && *(line->ptr + line->len -1) == '\r') {
			line->len--;  /* remove optional \r */
		}
		line->ptr[line->len] = 0; //½«»»ÐÐ·ûÌæ»»³ÉÖÕÖ¹·û
	}
	else /*last line ends without newline */
	{
		*line = *src;
		src->ptr += src->len;
		src->len = 0;
	}

	return STATUS_SUCCESS;
}

u32_t mxLexparser::extractToken(chunk_t *token, const char termination, chunk_t *src)
{
	u8_t *eot = (u8_t *)memchr(src->ptr, termination, src->len);

	if (termination == ' ')
	{
		u8_t *eot_tab = (u8_t *)memchr(src->ptr, '\t', src->len);

		/* check if a tab instead of a space terminates the token */
		eot = ( eot_tab == NULL || (eot && eot < eot_tab) ) ? eot : eot_tab;
	}

	/* initialize empty token */
	token->ptr = NULL;
	token->len = 0;

	if (eot == NULL) {/* termination symbol not found */
		return STATUS_FAILED;
	}
	/* extract token */
	token->ptr = src->ptr;
	token->len = (u32_t)(eot - src->ptr);

	/* advance src pointer after termination symbol */
	src->ptr = eot + 1;
	src->len -= (token->len + 1);

	return STATUS_SUCCESS;

}

bool mxLexparser::eatWhitespace(chunk_t *src)
{
	while (src->len > 0 && (*src->ptr == ' ' || *src->ptr == '\t')) {
		src->ptr++;  src->len--;
	}
	
	return  src->len > 0 && *src->ptr != '#';
}

