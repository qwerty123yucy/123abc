#ifndef	__MALLOC_H
#define	__MALLOC_H

#include <stdbool.h>
#include <bits.h>
#include <stddef.h>
#include <stdint.h>

#define BLOCK_ALIGN_BITS	(3)
#define BLOCK_USED		(U(1))
#define BLOCK_FREE		(U(0))
#define BLOCK_ALIGN		((1) << (BLOCK_ALIGN_BITS))


#define HEAD_SIZE		(sizeof(uint32_t))

#define BLOCK_SIZE_MASK		((~U(0)) << BLOCK_ALIGN_BITS)
#define BLOCK_FLAG_MASK 	(~BLOCK_SIZE_MASK)

#define get_block_num(x)	((((x) / (BLOCK_ALIGN)) + 1) + (((x) % (BLOCK_ALIGN))?(1):(0)))
#define get_block_size(x)	((x) & BLOCK_SIZE_MASK)
#define get_block_mask(x)	((x) & BLOCK_FLAG_MASK)
#define get_payload_size(x)	((get_block_size(x)) - (2) * HEAD_SIZE) 


#define block_used(x)		((x) & (BLOCK_USED))
#define block_free(x)		(block_used(x) == 0)


#define CHUNK_SIZE		4096


void memset(const void *addr, const int c, uint32_t sz);
void memcpy(const void *dst, const void *src, uint32_t sz);

void *mem_alloc(uint32_t size);
void mem_free(void *);
void *mem_realloc(void *addr, uint32_t sz);


#endif
