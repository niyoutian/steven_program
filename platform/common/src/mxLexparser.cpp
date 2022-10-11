#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
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
	
	if (extractToken(src, '\n', line) == STATUS_SUCCESS)
	{
		if (line->len > 0 && *(line->ptr + line->len -1) == '\r') {
			line->len--;  /* remove optional \r */
		}
		line->ptr[line->len] = 0; //将换行符替换成终止符
	}
	else /*last line ends without newline */
	{
		*line = *src;
		src->ptr += src->len;
		src->len = 0;
	}

	return STATUS_SUCCESS;
}

u32_t mxLexparser::extractToken(chunk_t *src, const char termination, chunk_t *token)
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

u32_t mxLexparser::extractFilenameSecret(chunk_t *src, chunk_t *filename, chunk_t *secret)
{
	char delimiter = ' ';
	
	if (!eatWhitespace(src)) {
		return STATUS_FAILED;
	}
	if (*src->ptr == '\'' || *src->ptr == '"')
	{
		delimiter = *src->ptr;
		src->ptr++;  src->len--;
	}
	if (extractToken(src, delimiter, filename)==STATUS_FAILED) {
		return STATUS_FAILED;
	}

	/* check for optional passphrase */
	if (eatWhitespace(src)) {
		if (eatWhitespace(src) == false) {
			printf("missing secret\n");
			return STATUS_FAILED;
		}
		
		delimiter = ' ';
		chunk_t raw_secret = {NULL, 0};
		bool quotes = false;
		
		if (*src->ptr == '\'' || *src->ptr == '"') {
			quotes = true; //代表有引号
			delimiter = *src->ptr;
			src->ptr++;  src->len--;
		}
		if (extractToken(src, delimiter, &raw_secret)==STATUS_FAILED) {
			if (delimiter == ' ') {
				raw_secret = *src;
			} else {
				printf("missing second delimiter\n");
				return STATUS_FAILED;
			}
		}
		if (quotes) {
			/* treat as an ASCII string */
			*secret = chunk_clone(raw_secret);
			return STATUS_SUCCESS;
		}
		/* treat 0x as hex, 0s as base64 */
		if (raw_secret.len > 2) {
			if (strncasecmp("0x", (const char*)raw_secret.ptr, 2) == 0) {
				//TODO
			}
			if (strncasecmp("0s", (const char*)raw_secret.ptr, 2) == 0) {
				//TODO
			}
		}
		*secret = chunk_clone(raw_secret);
		return STATUS_SUCCESS;

	}
	return STATUS_SUCCESS;
}


bool mxLexparser::eatWhitespace(chunk_t *src)
{
	while (src->len > 0 && (*src->ptr == ' ' || *src->ptr == '\t')) {
		src->ptr++;  src->len--;
	}
	
	return  src->len > 0 && *src->ptr != '#';
}

/**
 * Converts a PEM encoded file into its binary form (RFC 1421, RFC 934)
 */
u32_t mxLexparser::convertPEM2BIN(chunk_t src, chunk_t &chunk)
{
#if 0
	chunk_t line = {NULL, 0};
	u32_t state  = PEM_PRE;
	
	while (fetchLine(&src, &line) == STATUS_SUCCESS) {
		if (state == PEM_PRE) {
			if (find_boundary("BEGIN", &line)) {
				state = PEM_MSG;
			}
			continue;
		}

	}
#endif
	return STATUS_SUCCESS;
}

