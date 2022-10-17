#include <utils/io.h>
#include <mm/mman.h>
#include <utils/shell.h>
#include <rtc.h>
#include <cpu.h>
#include <stm32f103.h>

// a toy without parse of special characters
uint32_t input_handle(const char *input_buf, uint32_t buf_sz){
	static uint32_t sz = 0; uint32_t end_sz = 0;
	char tmp; char *p = (char *)input_buf + sz;
	for(;;){
		//buf size is enough. return 0 to let main loop call realloc
		if(sz > buf_sz - 2){
			return 0;	
		}
		else{
			tmp = getc();
			if(tmp == '\r'){
				*p++ = '\n';
				*p = '\0';
				end_sz = sz + 2;    //the '\n' and '\0' is counted
				sz = 0;
				print_f("\n");
				return end_sz;

			}
			else{
				*p++ = tmp;
				sz++;
				print_f("%c", tmp);
			}
		}
	}
}

// a toy, now just can test usart in/out
int input_parse(const char *input_buf, uint32_t sz){
	if(input_buf[0] == 'q' && input_buf[1] == '\n'){
		return -1;
	}
	else{
		print_f("%s", input_buf);
	}
	return 0;
}

int main_loop(){
	print_f("hello stm32!\n");
	uint32_t rtc_cnt = rtc_read_cnt();
	print_f("rtc_cnt: %lx\n", rtc_cnt);
	uint32_t buf_block = BUF_BLOCK_SIZE; uint32_t buf_sz = buf_block;
	char *input_buf = (char *)mem_alloc(sizeof(char) * buf_sz);
	uint32_t sz = 0;
	char *msg = "[shell]: "; int ret;
	if(input_buf == NULL){
		print_f("failed to init shell!\n");
		return -1;
	}
	for(;;){
		print_f("%s", msg);
		while (sz = input_handle(input_buf, buf_sz) == 0){
			buf_sz += buf_block;
			input_buf = mem_realloc(input_buf, buf_sz);
			if(input_buf == NULL){
				print_f("failed to realloc buf for input!\n");
				return -1;
			}
		}


		ret = input_parse(input_buf, sz);
		if(ret == -1){
			break;
		}
	}
	mem_free(input_buf);
	return 0;
}
