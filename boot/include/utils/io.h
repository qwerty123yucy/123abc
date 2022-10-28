#ifndef	__IO_H
#define	__IO_H

#include <stdbool.h>
#include <stdint.h>


char getc();
void putc(char c);

int print_f(const char *format, ...);
uint32_t puts(char *data);
uint32_t put_word_uint(uint32_t data);
void put_word_hex(uint32_t data);
bool format_fit(const char *str, const char *t);


#endif
