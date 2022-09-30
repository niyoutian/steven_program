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


