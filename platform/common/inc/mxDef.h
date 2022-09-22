#ifndef __MXDEF_H__
#define __MXDEF_H__
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

typedef char                    s8_t;
typedef unsigned char           u8_t;
typedef signed short            s16_t;
typedef unsigned short          u16_t;
typedef signed int              s32_t;
typedef unsigned int            u32_t;
typedef signed long long        s64_t;
typedef unsigned long long      u64_t;

typedef struct  {
	u8_t *ptr; /** Pointer to start of data */
	u32_t len; /** Length of data in bytes */
} chunk_t;

/**
 * Empty chunk.
 */
//chunk_t chunk_empty = { NULL, 0 };

/**
 * Create a new chunk pointing to "ptr" with length "len"
 */
static inline chunk_t chunk_create(u8_t *ptr, size_t len)
{
	chunk_t chunk = {ptr, len};
	return chunk;
}

/**
 * Allocate a chunk on the heap
 */
#define chunk_alloc(bytes) ({size_t x = (bytes); chunk_create(x ? malloc(x) : NULL, x);})

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


#endif
