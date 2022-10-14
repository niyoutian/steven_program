#ifndef __MXDEF_H__
#define __MXDEF_H__
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

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




#endif
