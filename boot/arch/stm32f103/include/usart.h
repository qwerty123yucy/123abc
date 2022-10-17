#ifndef __USART_H
#define __USART_H

#include <stm32f103.h>
#include <stdint.h>

// usart registers
#define USART_SR        (0x00)
#define USART_SR_RXNE   (1 << 5)
#define USART_SR_TXE    (1 << 7)
#define USART_SR_TC	(1 << 6)


#define USART_DR        (0x04)
#define USART_DR_DATA_MASK      (0xff)



#define USART_BRR       (0x08)



#define USART_CR1       (0x0c)

#define USART_OVER8     (1 << 15)       //0: set oversampling bits = 16 else 8
#define USART_CR1_UE    (1 << 13)       //1: enable serial
#define USART_CR1_M     (1 << 12)       //0: length 8   (using CR2's 'stop' to set the end bits) 
#define USART_CR1_PCE   (1 << 10)       //0: disable check      1: enable check
#define USART_CR1_PS    (1 << 9)        //0: even check         1: odd check
#define USART_CR1_TXIE  (1 << 7)        //when TX buffer turns to empty 0: do not generate an interrupt 
                                        //                              1: generate an interrupt

#define USART_CR1_TCIE  (1 << 6)        //when there is no data to send 0: do not  generate an interrupt
                                        //                              1: generate an interrupt

#define USART_CR1_RXNEIE        (1 << 5)//when there is data in RX      0: do not generate an interrupt
                                        //                              1: generate an interrupt

#define USART_CR1_TE    (1 << 3)        //enable TX (sending)
#define USART_CR1_RE    (1 << 2)        //enable RX (receiving)




#define USART_CR2       (0x10)

#define USART_CR2_STOP  (3 << 12)       //0: one stop bit



#define USART_CR3       (0x14)
#define USART_GTPR      (0x18)


// usart user setting values
#define BAUD_RATE       (115200)
#define DATA_BITS       (8)
#define STOP_BITS       (1)
#define OVERSAMPLING    (16)


struct usart_t {
	uint32_t base_addr;
	uint32_t baud_rate;
	uint32_t data_bits;
	uint32_t stop_bits;
	uint32_t oversampling;
};


void init_usart();
void usart_send_char(char c, struct usart_t *dev);
char usart_get_char(struct usart_t *dev);
char getc();
void putc(char c);

#endif

