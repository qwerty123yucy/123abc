#include <usart.h>
#include <utils/io.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool format_fit(const char *str, const char *t){
	int index = 0;
	while(str[index] != '\0' && t[index] != '\0' && str[index] == t[index]){
		index++;
	}
	if(t[index] == '\0'){
		return true;
	}
	return false;
}

// a toy. just use this to simply print something
int print_f(const char *format, ...){
	va_list args;
	va_start(args, format);
	char *p = (char *)format; char *str = NULL; uint32_t val; char c;
	uint32_t sz = 0;

	while(*p != '\0'){
		if(*p != '%'){
			if(*p == '\n'){
				putc('\r');
				sz++;
			}
			putc(*p);
			sz++;
		}
		else{
			p++;
			if(format_fit(p, "s")){
				str = va_arg(args, char *);
				sz += puts(str);
			}
			else if(format_fit(p, "lx")){
				val = va_arg(args, uint32_t);
				put_word_hex(val);
				sz += sizeof(uint32_t)*2 + 2;

				p+=1;
			}
			else if(format_fit(p, "c")){
				c = (char)va_arg(args, int);
				putc(c);
				sz++;
			}
			else{
				putc('%');
				sz++;
				p--;
			}
		}
		p++;
	}
	va_end(args);
	return sz;

}

uint32_t puts(char *data){
        uint32_t sz = 0;
        char *p = data;
        while(*p != '\0'){
                if(*p == '\n'){
                        putc('\r');
                        sz++;
                }
                putc(*p++);
                sz++;
        }
        return sz;
}

void put_word_hex(uint32_t data){
        putc('0'); putc('x');
        for(int i = 0;i < 2 * sizeof(uint32_t);i++){
                uint32_t tmp = (data >> (28 - 4*i)) & 0xf;
                if(tmp < 10){
                        putc('0' + tmp);
                }
                else{
                        switch(tmp){
                                case 10:putc('a');break;
                                case 11:putc('b');break;
                                case 12:putc('c');break;
                                case 13:putc('d');break;
                                case 14:putc('e');break;
                                case 15:putc('f');break;
                                default:
                                        break;

                        }
                }
        }
        return;
}



