#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <mm/mman.h>
#include <utils/io.h>

static uint32_t *malloc_pool_front = NULL;
static uint32_t *malloc_pool_real = NULL;
static bool mem_inited = false;


void memset(const void *addr, const int c, uint32_t sz){
	char *p = (char *)addr;
	while(sz--){
		*p++ = (char)c;
	}
	return;
}

int memcmp(const void *str1, const void *str2, uint32_t sz){
	char *p_str1 = (char *)str1;
	char *p_str2 = (char *)str2;
	while(sz--){
		if((*p_str1 != *p_str2)){
			return (int)(*p_str1) - (int)*p_str2;
		}		
		p_str1++;
		p_str2++;
	}
	return 0;
}

char *strchr(const char *str, const int c){
	char *p = (char *)str;
	while(*p != '\0' && *p != (char)c){
		p++;
	}
	if(*p == '\0'){
		p = NULL;
	}
	return p;
}

uint32_t strlen(const char *str){
	char *p = (char *)str;
	while(*p != '\0'){
		p++;
	}
	return (uint32_t)(p - str);
}

void memcpy(const void *dst, const void *src, uint32_t sz){
	char *p_dst = (char *)dst;
	char *p_src = (char *)src;
	while(sz--){
		*p_dst++ = *p_src++;
	}
	return;
}

/*
 * increase heap end by the give size (must be 4 bytes aligned)
 * mem_alloc call this to create or increase malloc pool
 */

void *mem_sbrk(int incr){
	extern char stack__end;
	void *heap_start = &stack__end;
	static void *heap_end = NULL;
	if(heap_end == NULL){
		heap_end = heap_start;
	}
	uint32_t heap_size = (char *)heap_end - (char *)heap_start;
	if(incr < 0 || incr % 4 || heap_size + incr > MAX_HEAP_SIZE){
		return NULL;
	}
	else{
		void *old_end = heap_end;
		heap_end = (void *)((char *)old_end + incr);
		return old_end;
	}
}

bool valid_head_size(uint32_t *head){
	return get_block_size(*head) > 0;	
}

bool valid_head_flag(uint32_t *head){
	return get_block_mask(*head) <= 1;
}

bool valid_head(uint32_t *head){
	return valid_head_size(head) && valid_head_flag(head);
}

/* 
 * get the addr of the next block head by the given block head
 * continously calling next head ends at malloc_pool_real 
 */
uint32_t *next_head(uint32_t *head){
	return (uint32_t *)((char *)head + get_block_size(*head));
}

/* 
 * with the head addr and the block size calculated from the value of head
 * we can get the tail of this block.
 * useful when handle free blocks
 */
uint32_t *block_tail(uint32_t *head){
	return (next_head(head) - 1);
}

uint32_t *get_head_of_tail(uint32_t *tail){
	return (uint32_t *)((char *)tail - get_block_size(*tail)) + 1;
}

/* 
 * call sbrk to get malloc pool and init the front, real 
 * and head of the free block with its tail filled
 */
void mem_init(){
	malloc_pool_front = (uint32_t *)mem_sbrk(CHUNK_SIZE);
	if(malloc_pool_front == NULL){
		return;
	}
	*malloc_pool_front = 0 | BLOCK_USED;

	uint32_t *head = malloc_pool_front + 1;

	*head = (CHUNK_SIZE - (1 << BLOCK_ALIGN_BITS)) | BLOCK_FREE;
	*block_tail(head) = *head;

	malloc_pool_real = next_head(head);
	*malloc_pool_real = 0 | BLOCK_USED;
	mem_inited = true;
	return;
}

void mem_free(void *addr){
	uint32_t *head = ((uint32_t *)addr) - 1;
	if(!valid_head(head) || block_free(*head)){
		return;
	}
	*head &= ~BLOCK_USED;
	*block_tail(head) = *head;

	uint32_t *right = next_head(head);
	uint32_t *left_tail = head - 1;

	if(valid_head(right) && block_free(*right)){
		*head = (get_block_size(*head) + get_block_size(*right)) | BLOCK_FREE;
		*block_tail(head) = *head;
	}

	if(valid_head(left_tail) && block_free(*left_tail)){
		uint32_t *left = get_head_of_tail(left_tail);
		*left = (get_block_size(*left) + get_block_size(*head)) | BLOCK_FREE;
		*block_tail(left) = *left;
	}
	return;
}

uint32_t *extend_malloc_pool(){
	void *start = mem_sbrk(CHUNK_SIZE);
	if(start == NULL){
		return NULL;
	}
	uint32_t *left_tail = malloc_pool_real - 1;
	uint32_t *new_head = NULL;
	if( block_used(*left_tail) ){
		new_head = malloc_pool_real;
		*new_head = CHUNK_SIZE | BLOCK_FREE;
		*block_tail(new_head) = *new_head; 
		malloc_pool_real = next_head(new_head);
		*malloc_pool_real = 0 | BLOCK_USED;
	}
	else{
		new_head = get_head_of_tail(left_tail);
		*new_head = (get_block_size(*new_head) + CHUNK_SIZE) | BLOCK_FREE;
		*block_tail(new_head) = *new_head;
		malloc_pool_real = next_head(new_head);
		*malloc_pool_real = 0 | BLOCK_USED;
	}
	return new_head;
}

void *mem_alloc(uint32_t size){
	if(mem_inited == false){
		mem_init();
		if(malloc_pool_front == NULL){
			return NULL;
		}
	}
	uint32_t *head = malloc_pool_front + 1;
	uint32_t target_block_size = get_block_num(size) << BLOCK_ALIGN_BITS;
	while(head != NULL){
		if(valid_head(head)){
			if(block_used(*head)){
				head = next_head(head);
			}
			else{
				if( get_block_size(*head) < target_block_size){
					head = next_head(head);
				}
				else{
					//  the target size is nearly the block size. directly use this block
					if( (get_block_size(*head) == target_block_size) || (get_block_size(*head) / (get_block_size(*head) - target_block_size)) > 4 ){
						*head |= BLOCK_USED;
						*block_tail(head) = *head;
					}
					//  split this block
					else{
						uint32_t initial_size = get_block_size(*head);
						*head = target_block_size | BLOCK_USED;
						*block_tail(head) = *head;
						*next_head(head) = (initial_size - target_block_size) | BLOCK_FREE;
						*block_tail(next_head(head)) = *next_head(head);
					}
					break;
				}
			}
		}
		else{
			//increase heap end and return the last free block's head
			head = extend_malloc_pool();
		}
	}
	if(head == NULL){
		return NULL;
	}
	else{
		return (void *)(head + 1);
	}


}

void *mem_realloc(void *addr, uint32_t sz){
	void *new_addr;
	uint32_t *head = (uint32_t *)addr - 1;
	uint32_t target_block_size;
	uint32_t old_block_size;
	if(!valid_head(head) || block_free(*head)){
		return NULL;
	}
	old_block_size = get_block_size(*head);
	target_block_size = get_block_num(sz) << BLOCK_ALIGN_BITS;
	if(old_block_size >= target_block_size){
		new_addr = addr;
	}
	else{
		new_addr = mem_alloc(sz);
		if(new_addr != NULL){
			memcpy(new_addr, addr, get_payload_size(*head));
			mem_free(addr);
		}

	}
	return new_addr;
}
