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

/**
 * extracts a key: value pair
 */
u32_t mxLexparser::extractKeyValue(chunk_t *src, chunk_t *key, chunk_t *value)
{
	/* extract name */
	if (extractToken(src, ':', key)==STATUS_FAILED) {
		return STATUS_FAILED;
	}

	/* extract value */
	char delimiter = ' ';

	if (!eatWhitespace(src)) {
		*value = chunk_empty;
		return STATUS_SUCCESS;
	}

	if (*src->ptr == '\'' || *src->ptr == '"') {
		delimiter = *src->ptr;
		src->ptr++;  src->len--;
	}
	if (extractToken(src, delimiter, value)==STATUS_FAILED) {
		if (delimiter == ' ') {
			*value = *src;
			src->len = 0;
		} else {
			return STATUS_FAILED;
		}
	}

	return STATUS_SUCCESS;
}

u32_t mxLexparser::extractFilenameSecret(chunk_t *src, chunk_t *filename, chunk_t *secret)
{
	char delimiter = ' ';

	secret->ptr = NULL;
	secret->len = 0;
	
	if (!eatWhitespace(src)) {
		return STATUS_FAILED;
	}
	if (*src->ptr == '\'' || *src->ptr == '"')
	{
		delimiter = *src->ptr;
		src->ptr++;  src->len--;
	}
	if (extractToken(src, delimiter, filename)==STATUS_FAILED) {
		if (delimiter == ' ') {
			filename->ptr = src->ptr;
			filename->len = src->len;
			src->ptr = NULL;
			src->len = 0;
		} else {
			return STATUS_FAILED;
		}
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


u32_t mxLexparser::analyseDataOfPEM(chunk_t line, u32_t &state, chunk_t *result)
{
	u32_t ret = 0;
	chunk_t data = chunk_empty;
	chunk_t name  = chunk_empty;
	chunk_t value = chunk_empty;
	bool encrypted = false;

	switch (state) {
		case PEM_PRE:
			if (findBoundary(&line, "BEGIN")) {
				state = PEM_MSG;
			}
			ret = STATUS_SUCCESS;
			break;
		case PEM_MSG:
			if (memchr(line.ptr, ':', line.len) == NULL) {
				state = PEM_BODY;
			} else {
				state = PEM_HEADER;
			}
			ret = STATUS_CONTINUE;
			break;

		case PEM_HEADER:
			//TODO
			/* an empty line separates HEADER and BODY */
			if (line.len == 0) {
				state = PEM_BODY;
				ret = STATUS_SUCCESS;
				break;
			}
			if (extractKeyValue(&line, &name, &value) == STATUS_FAILED) {
				ret = STATUS_FAILED;
				break;
			}
			if ( (name.len == strlen("Proc-Type")) && (strncmp((const char*)name.ptr, "Proc-Type", strlen("Proc-Type")) == 0) 
				&& value.len && *value.ptr == '4') {
				encrypted = true;
			}
			break;
		case PEM_BODY:
			if (findBoundary(&line, "END")) {
				state = PEM_POST;
				ret = STATUS_SUCCESS;
				break;
			}

			/* remove any trailing whitespace */
			if (extractToken(&line ,' ', &data) == STATUS_FAILED) {
				data = line;
			}
			
			data = chunk_from_base64(data, result->ptr);
			result->ptr += data.len;
			result->len += data.len;

			ret = STATUS_SUCCESS;
			break;
		default:
			ret = STATUS_FAILED;
			break;
	}

	return ret;
}
/**
 * Converts a PEM encoded file into its binary form (RFC 1421, RFC 934)
 */
u32_t mxLexparser::convertPEM2BIN(chunk_t &src)
{
	bool encrypted = false;
	chunk_t line = {NULL, 0};
	u32_t state  = PEM_PRE;
	chunk_t result = chunk_empty;
	result.ptr = src.ptr;
	chunk_t blob = src;
	u32_t  ret = 0;
	
	while (fetchLine(&blob, &line) == STATUS_SUCCESS) {
		do {
			ret = analyseDataOfPEM(line, state, &result);
			if (ret == STATUS_FAILED) {
				return STATUS_FAILED;
			} 
		} while(ret == STATUS_CONTINUE);
	}

	printf("state = %u\n",state);
	src.len = result.len;
	if (state != PEM_POST) {
		printf("  file coded in unknown format, discarded\n");
		return STATUS_FAILED;
	}

	if (encrypted == true) {
		//TODO
	}
	return STATUS_SUCCESS;
}

/**
 * find a boundary of the form -----tag name-----
 */
bool mxLexparser::findBoundary(chunk_t *line, char* tag)
{
	if ( (line->len < strlen("-----")) || (strncmp((const char*)line->ptr, "-----", strlen("-----")) != 0) ) {
		return false;
	}
	*line = chunk_skip(*line, strlen("-----"));
	
	if ( (line->len < strlen(tag)) || (strncmp((const char*)line->ptr, tag, strlen(tag)) != 0) ) {
		return false;
	}
	*line = chunk_skip(*line, strlen(tag));
	
	if ( (line->len < 1) || (*line->ptr != ' ') ) {
		return false;
	}
	*line = chunk_skip(*line, 1);

	/* extract name */
	chunk_t name = chunk_empty;
	name.ptr = line->ptr;
	char * pos = strstr((char*)line->ptr,"-----");
	if (pos != NULL) {
		name.len = (u32_t)((u8_t *)pos - line->ptr);
		printf("  -----%s %.*s-----\n", tag, (int)name.len, name.ptr);
		*line = chunk_skip(*line, name.len);
		return true;
	}

	return false;
}
