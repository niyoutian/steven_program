#ifndef __CHUNK_H__
#define __CHUNK_H__
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "mxDef.h"


/**
 * Empty chunk.
 */
extern chunk_t chunk_empty;

chunk_t chunk_skip(chunk_t chunk, size_t bytes);


/**
 * Create a new chunk pointing to "ptr" with length "len"
 */
static inline chunk_t chunk_create(u8_t *ptr, size_t len)
{
	chunk_t chunk = {ptr, len};
	return chunk;
}

static inline chunk_t chunk_clone(chunk_t chunk)
{
	chunk_t clone = {NULL, 0};
	
	if (chunk.ptr && chunk.len > 0) {
		clone.ptr = (u8_t *)malloc(chunk.len);
		clone.len = chunk.len;
		memcpy(clone.ptr, chunk.ptr, chunk.len);
	}
	return clone;
}


/**
 * Free contents of a chunk
 */
static inline void chunk_free(chunk_t *chunk)
{
	free(chunk->ptr);
	chunk->ptr = NULL;
	chunk->len = 0;
}


/**
 * Allocate a chunk on the heap
 */
#define chunk_alloc(bytes) ({size_t x = (bytes); chunk_create(x ? (u8_t *)malloc(x) : NULL, x);})

static inline void chunk_printf(chunk_t chunk)
{
	printf("chunk ptr:%p,len=%u",chunk.ptr,chunk.len);
	for (u32_t i = 0; i < chunk.len; i++) {
		if (i % 16 == 0) {
			printf("\n");
		}
		printf("%02x ",*((u8_t *)chunk.ptr+i));
	}
}


chunk_t chunk_from_base64(chunk_t base64, u8_t *buf);




#endif
