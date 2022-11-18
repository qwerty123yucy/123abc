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


// arg paese functions for builtin command
int create_file_arg_parse(char *arg_out, const char *arg_in){
	char *p_start = strchr(arg_in, ' ');
	char *p_end = NULL;

	if(p_start == NULL){
		return -1;
	}
	while(*p_start == ' '){
		p_start++;
	}
	p_end = p_start;
	while(('a'<= *p_end && *p_end <= 'z') 
			|| ('A' <= *p_end && *p_end <= 'Z') 
			|| ('0' <= *p_end && *p_end <= '9') 
			|| (*p_end == '.')){
		p_end++;
	}
	if((*p_end != '\n') || (p_end == p_start) || ((p_end == p_start + 1) && (*p_start == '.')) || (uint32_t)(p_end - p_start) > MAX_FILE_NAME_SIZE){
		return -1;
	}

	arg_out[0] = '0'; arg_out[1] = ':';
	for(int i = 0;i < p_end - p_start;i++){
		arg_out[i + 2] = p_start[i];
	}
	arg_out[p_end - p_start + 2] = '\0';
	
	return 0;	
}


// exec functions for builtin command
int time_exec(const char *arg){
	if(arg != NULL){
		memset(arg, 0, MAX_ARG_SIZE);	
	}
        put_current_time(); print_f("\n");
        return 0;
}

int date_exec(const char *arg){
        if(arg != NULL){
                memset(arg, 0, MAX_ARG_SIZE);
        }
        put_current_date(); print_f("\n");
        return 0;
}

int temp_exec(const char *arg){
        if(arg != NULL){
                memset(arg, 0, MAX_ARG_SIZE);
        }
	print_f("%u\n", (uint32_t)adc_get_temp());
	return 0;
}

int dma_buf_exec(const char *arg){
	if(arg != NULL){
                memset(arg, 0, MAX_ARG_SIZE);
        }
	extern uint32_t dma_buffer[0xf];
	for(int i = 0;i < 0xf;i++){
        	print_f("%lx\n", dma_buffer[i]);
        }
	return 0;
}

int create_file_exec(const char *arg){
        if(arg == NULL){
        	return -1;
	}
	FIL fp;
	uint32_t written_sz;
	uint32_t ret = f_open(&fp, arg, FA_WRITE | FA_CREATE_ALWAYS);
	if(ret){
		print_f("f_open failed! ret = %lx\n", ret);
	}
	else{
		ret = f_write(&fp, "test_content", 13, (UINT*)(&written_sz));
		if(ret){
			print_f("failed to write content to file! ret = %lx\n", ret);
		}
		ret = f_close(&fp);
		if(ret){
			print_f("failed to sync file to disk! ret = %lx\n", ret);
		}
	}
	return 0;
}

int read_file_exec(const char *arg){
        if(arg == NULL){
                return -1;
        }
        FIL fp;
	char *file_recv_buf[20]; uint32_t read_sz;
        uint32_t ret = f_open(&fp, arg, FA_READ);
        if(ret){
                print_f("f_open failed! ret = %lx\n", ret);
        }
        else{
                ret = f_read(&fp, file_recv_buf, 20, (UINT*)(&read_sz));
                if(ret){
                        print_f("failed to read file content! ret = %lx\n", ret);
                }
		file_recv_buf[19] = '\0';
		print_f("%s\n", file_recv_buf);
                ret = f_close(&fp);
                if(ret){
                        print_f("failed to close file! ret = %lx\n", ret);
                }
        }
        return 0;
}


int mount_exec(const char *arg){
        if(arg != NULL){
                memset(arg, 0, MAX_ARG_SIZE);
        }
	static FATFS fs;
	uint32_t ret = (uint32_t)f_mount(&fs, "0:", (BYTE)1);
	if(ret){
		print_f("failed to mount! ret = %lx\n", ret);
	}
        return 0;
}

int mkfs_exec(const char *arg){
        if(arg != NULL){
                memset(arg, 0, MAX_ARG_SIZE);
        }

	print_f("mkfs spi_flash...\n");
	char *work = mem_alloc(FF_MAX_SS);
	if(work == NULL){
		print_f("failed to alloc work buf for mkfs!\n");
	}
	else{
		uint32_t ret = f_mkfs("0:", NULL, work, FF_MAX_SS);
		if(ret){
			print_f("failed to mkfs! ret = %lx\n", ret);
		}
		mem_free(work);
	}
	return 0;
}



static struct builtin_cmd cmd_list[BUILTIN_CMD_NUM] = {
	{
		.name = "time",
		.arg_parse = NULL,
		.exec = time_exec,
	},
	{
                .name = "date",
                .arg_parse = NULL,
                .exec = date_exec,
        },
	{
                .name = "temp",
                .arg_parse = NULL,
                .exec = temp_exec,
        },
	{
                .name = "create_file",
                .arg_parse = create_file_arg_parse,
                .exec = create_file_exec,
        },
	{
                .name = "dma_buf",
                .arg_parse = NULL,
                .exec = dma_buf_exec,
        },
	{
                .name = "mount",
                .arg_parse = NULL,
                .exec = mount_exec,
        },
	{
                .name = "read_file",
                .arg_parse = create_file_arg_parse,
                .exec = read_file_exec,
        },
	{
                .name = "mkfs",
                .arg_parse = NULL,
                .exec = mkfs_exec,
        },


};



// a toy without parse of special characters
static int input_handle(const char *input_buf, uint32_t buf_sz){
	static uint32_t sz = 0; uint32_t end_sz = 0;
	char tmp; char *p = (char *)input_buf + sz;
	for(;;){
		if(sz > buf_sz - 2){
			//buf size is not enough. return 0 to let main loop call realloc
			return -1;	
		}
		else{
			tmp = getc();
			switch(tmp){
				case '\r':
					*p++ = '\n';
					*p = '\0';
					end_sz = sz + 2;    //the '\n' and '\0' is counted
					sz = 0;
					print_f("\n");
					return (int)end_sz;
				case (char)(0x03):
					print_f("\n");
					sz = 0;
					return (int)sz;
				default:
					*p++ = tmp;
	                                sz++;
        	                        print_f("%c", tmp);
					break;
			}
		}
	}
}


static int input_parse(const char *input_buf, uint32_t sz){
	if((sz < 2) || (input_buf[0] == 'q' && input_buf[1] == '\n')){
		return -1;
	}
	else{	
		char *arg_out = NULL;
		for(uint32_t i = 0;i < BUILTIN_CMD_NUM;i++){
			if(format_fit(input_buf, cmd_list[i].name) 
					&& ((input_buf[strlen(cmd_list[i].name)]) == '\n' 
						|| (input_buf[strlen(cmd_list[i].name)]) == ' ')) {
				if(cmd_list[i].arg_parse != NULL){
					arg_out = (char *)mem_alloc(MAX_ARG_SIZE);
					if(cmd_list[i].arg_parse(arg_out, input_buf)){
						print_f("bad args for command: %s\n", cmd_list[i].name);
						return 0;
					}
				}
				if(cmd_list[i].exec == NULL || cmd_list[i].exec(arg_out)){
					print_f("failed to exec command: %s\n", cmd_list[i].name);
				}
				if(arg_out != NULL){
					mem_free(arg_out);
				}
				return 0;
			}
		}
		print_f("no a builtin command\n");
		
		return 0;
	}
}


int main_loop(){
	print_f("hello stm32!\n");
	uint32_t buf_sz = INPUT_BUF_BLOCK_SIZE;
	char *input_buf = (char *)mem_alloc(sizeof(char) * buf_sz);
	int sz = 0;
	char *msg = "[shell]: "; int ret;
	if(input_buf == NULL){
		print_f("failed to init shell!\n");
		return -1;
	}
	for(;;){
		print_f("%s", msg);
		while ((sz = input_handle(input_buf, buf_sz)) == -1){
			buf_sz += INPUT_BUF_BLOCK_SIZE;
			char *old_buf = input_buf;
			input_buf = mem_realloc(input_buf, buf_sz);
			if(input_buf == NULL){
				mem_free((void *)old_buf);
				print_f("failed to realloc buf for input!\n");
				return -1;
			}
		}
		if(sz == 0){
			continue;
		}
		ret = input_parse(input_buf, (uint32_t)sz);
		if(ret == -1){
			break;
		}
	}
	print_f("terminated!\n");
	mem_free(input_buf);
	return 0;
}
