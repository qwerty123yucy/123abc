#include <utils/io.h>
#include <mm/mman.h>
#include <utils/shell.h>
#include <rtc.h>
#include <cpu.h>
#include <stm32f103.h>
#include <stdbool.h>
#include <utils/time.h>
#include <adc.h>
#include <spi_flash.h>

// a toy without parse of special characters
uint32_t input_handle(const char *input_buf, uint32_t buf_sz){
	static uint32_t sz = 0; uint32_t end_sz = 0;
	char tmp; char *p = (char *)input_buf + sz;
	for(;;){
		//buf size is not enough. return 0 to let main loop call realloc
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

extern uint32_t dma_buffer[0xf];

static char flash_buf[4096] = {0};
static char flash_recv_buf[4096] = {0};

// a toy, now just can test usart in/out
int input_parse(const char *input_buf, uint32_t sz){
	if(input_buf[0] == 'q' && input_buf[1] == '\n'){
		return -1;
	}
	else{
		if(format_fit(input_buf, "temp\n")){
			print_f("%u\n", (uint32_t)adc_get_temp());
			
		}
		else if(format_fit(input_buf, "time\n")){
			put_current_time(); print_f("\n");
			
		}
		else if(format_fit(input_buf, "date\n")){
			put_current_date(); print_f("\n");

		}
		else if(format_fit(input_buf, "dma_buf\n")){
                        for(int i = 0;i < 0xf;i++){
				print_f("%lx\n", dma_buffer[i]);
			}
                }
		else if(format_fit(input_buf, "flash_jedec\n")){
			print_f("%lx\n", spi_flash_read_jedec());
		}
		else if(format_fit(input_buf, "flash_write\n")){
                        print_f("writing to spi_flash...\n");
			for(int i = 0;i<4096;i++){
				flash_buf[i] = i%26 + 'a';
			}
			spi_flash_write((uint32_t)10, flash_buf, 4096, true);
                }
		else if(format_fit(input_buf, "flash_chip_erase\n")){
                        print_f("erasing spi_flash...\n");
                        spi_flash_chip_erase();
                }

		else if(format_fit(input_buf, "flash_read\n")){
			spi_flash_read((uint32_t)0, flash_recv_buf, 4096);
			flash_recv_buf[4095] = '\0';
                        print_f("%s\n", flash_recv_buf);
                }

		else{
			print_f("not a builtin command!\n");
		}
	}
	return (int)sz;
}

int main_loop(){
	print_f("hello stm32!\n");
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
		while ((sz = input_handle(input_buf, buf_sz)) == 0){
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
