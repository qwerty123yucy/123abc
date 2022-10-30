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
#include <ff.h>

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


static FATFS fs;
static FIL fp;



static char flash_buf[3000] = {0};
static char flash_recv_buf[3000] = {0};
static char work[FF_MAX_SS];

static char *file_content = "file_content";
static uint32_t written_sz = 0;
static uint32_t read_sz = 0;
static char file_read_buf[15] = {0};

// a toy, now just can act as a test
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
			for(int i = 0;i<3000;i++){
				if(i == 2999){
					flash_buf[i] = '\0';
				}
				else{
					flash_buf[i] = i % 10 + '0';
				}
			}
			spi_flash_write((uint32_t)0, flash_buf, 3000, true);
                }
		else if(format_fit(input_buf, "flash_chip_erase\n")){
                        print_f("erasing spi_flash...\n");
                        spi_flash_chip_erase();
                }

		else if(format_fit(input_buf, "flash_read\n")){
			spi_flash_read((uint32_t)0, flash_recv_buf, 3000);
                        flash_recv_buf[2999] = '\0';
			//print_f("%s\n", flash_recv_buf);
			for(int i = 0; i< 3000; i++){
				print_f("%c", flash_recv_buf[i]);
			}
                }
		else if(format_fit(input_buf, "flash_mkfs\n")){
			print_f("mkfs spi_flash...\n");
			uint32_t ret = (uint32_t)f_mkfs("0:", NULL, work, FF_MAX_SS);
			if(ret){
				print_f("failed to mkfs! ret = %lx\n", ret);
			}
                }
		else if(format_fit(input_buf, "flash_mount\n")){
                        uint32_t ret = (uint32_t)f_mount(&fs, "0:", (BYTE)1);
                        if(ret){
                                print_f("failed to mount! ret = %lx\n", ret);
                        }
                }
		else if(format_fit(input_buf, "flash_save_file\n")){
			uint32_t ret;
                        ret = (uint32_t)f_open(&fp, "0:test.txt", FA_WRITE | FA_CREATE_ALWAYS);
                        if(ret){
                                print_f("failed to open file to write! ret = %lx\n", ret);
                        }
			ret = (uint32_t)f_write(&fp, file_content, 13, (UINT*)(&written_sz));
			if(ret){
				print_f("failed to write to file!\n");
			}
			ret = (uint32_t)f_close(&fp);
			if(ret){
				print_f("failed to sync file to flash!\n");
			}
			else{
				print_f("file successfully written.\n");
			}
                }
		else if(format_fit(input_buf, "flash_read_file\n")){
			uint32_t ret = (uint32_t)f_open(&fp, "0:test.txt", FA_READ);
                        if(ret){
                                print_f("failed to open file to read! ret = %lx\n", ret);
                        }
                        ret = f_read(&fp, file_read_buf, 13, (UINT*)(&read_sz));
                        if(ret){
                                print_f("failed to write to file!\n");
                        }
			else{
				print_f("%s\n", file_read_buf);
			}
			f_close(&fp);
                }
		else if(format_fit(input_buf, "flash_test\n")){
                        print_f("testing flash...\n");
                        for(int i = 0;i < W25X_CAPACITY;i += 3000){
				for(int j = 0;j<3000;j++){
					flash_buf[j] = (i/3000) % 26 + 'A';
				}
				
                        	spi_flash_write((uint32_t)i, flash_buf, 3000, true);
				spi_flash_read((uint32_t)i, flash_recv_buf, 3000);
				if(memcmp(flash_buf, flash_recv_buf, ((i+3000>W25X_CAPACITY)?(W25X_CAPACITY - i):3000)) != 0){
					print_f("test failed!\n");
					flash_buf[((i+3000>W25X_CAPACITY)?(W25X_CAPACITY - i):3000) -1] = '\0';
					flash_recv_buf[((i+3000>W25X_CAPACITY)?(W25X_CAPACITY - i):3000) - 1] = '\0';
					print_f("%s\n\n%s\n", flash_buf, flash_recv_buf);
				}
				else{
					print_f("ok when i = %lx\n", (uint32_t)i);
				}
			}
                }







		else{
			print_f("not a builtin command!\n");
		}
	}
	return (int)sz;
}

int main_loop(){
	print_f("hello stm32!\n");
	uint32_t buf_sz = INPUT_BUF_BLOCK_SIZE;
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
			buf_sz += INPUT_BUF_BLOCK_SIZE;
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
