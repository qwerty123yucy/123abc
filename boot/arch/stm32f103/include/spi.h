#ifndef __SPI_H
#define __SPI_H

// SPI registers are all 16-bit registers
#include <stdint.h>

#define SPI_CR1				(0x00)
#define SPI_CR1_BIDMODE			(1U << 15)		// 1: one line data mode
#define SPI_CR1_BIDIOE			(1U << 14)		// when on bidmode, 1: output
#define SPI_CR1_CRCEN			(1U << 13)		// must be wrtten when SPI is disabled
#define SPI_CR1_CRCNEXT			(1U << 12)		// if CRC is enabled, as the last data is written to DR, set it to transfer CRC value as the next data
#define SPI_CR1_DFF			(1U << 11)		// 0: 8-bit data   1: 16-bit
#define SPI_CR1_RXONLY			(1U << 10)		// when unimode,  1: receive only
#define SPI_CR1_SSM			(1U << 9)		// software slave management
#define	SPI_CR1_SSI			(1U << 8)		// internal slave select
#define SPI_CR1_LSBFIRST		(1U << 7)		// 1: LSB transmitted first
#define SPI_CR1_SPE			(1U << 6)		// enable this SPI
#define SPI_CR1_BR_SHIFT(x)		((x) << 3)		// baud rate  000: pclk/2 ...  111: pclk/256
#define SPI_CR1_BR_MASK			(SPI_CR1_BR_SHIFT(0b111))
#define SPI_CR1_MSTR			(1U << 2)		// 1: master 0: slave
#define SPI_CR1_CPOL			(1U << 1)		// 0: CK level 0 when idle. 1: ...
#define SPI_CR1_CPHA			(1U << 0)		// 0: first clock as data capture edge, 1: second ... 


#define SPI_CR2				(0x04)
#define SPI_CR2_TXEIE			(1U << 7)		// tx empty interrupt
#define SPI_CR2_RXNEIE			(1U << 6)		// data in rx interrupt 
#define SPI_CR2_ERRIE			(1U << 5)		// error(like CRC ... ) interrupt
#define SPI_CR2_SSOE			(1U << 2)		// in master mode, 1: SS output(to select slave) 0: input (multimaster???)
#define SPI_CR2_TXDMAEN			(1U << 1)		// tx buffer DMA
#define SPI_CR2_RXDMAEN			(1U << 0)		// rx buffer DMA


#define SPI_SR				(0x08)
#define SPI_SR_BSY			(1U << 7)		// busy communicating or TX not empty
#define SPI_SR_OVR			(1U << 6)		// overrun occured
#define SPI_SR_MODF			(1U << 5)		// mode fault
#define SPI_SR_CRCERR			(1U << 4)		// CRC error
#define SPI_SR_UDR			(1U << 3)		// underrun
#define SPI_SR_CHSIDE			(1U << 2)		// no use in SPI mode
#define SPI_SR_TXE			(1U << 1)		// TX empty, can write
#define SPI_SR_RXNE			(1U << 0)		// RX not empty, can read


#define SPI_DR				(0x0c)			// data register, 8bit LSB when dataframe is 8bit. 16bit LSB when dataframe is 16 bit
#define SPI_DR_MASK			(0xffff)	


#define SPI_CRCPR			(0x10)			// CRC polinomial (0007h is the reset value)

#define SPI_RXCRCR			(0x14)			// contains the CRC value of received bytes when CRC is enabl

#define SPI_TXCRCR			(0x18)			// contains the CRC value of sent bytes when CRC is enable

// some I2S registers temporary not used


enum spi_periph {spi1 = SPI1_BASE, spi2 = SPI2_BASE, spi3 = SPI3_BASE};
enum spi_trans_mode {bidi_out, bidi_in, full_duplex, unidi_rxonly};
enum spi_mode {spi_slave, spi_master};
enum spi_cs_mode {spi_hardware, spi_software};
enum spi_baudrate {spi_pclkdiv2 = 0x0, spi_pclkdiv4, spi_pclkdiv8, spi_pclkdiv16, spi_pclkdiv32, spi_pclkdiv64, spi_pclkdiv128, spi_pclkdiv256};
enum spi_clk_mode {idle_high_fst, idle_low_fst, idle_high_snd, idle_low_snd};
enum spi_dataframe {spi_8l, spi_8h, spi_16l, spi_16h};



struct spi_cfg {
	enum spi_mode mode;
	enum spi_trans_mode trans_mode;
	enum spi_cs_mode cs_mode;
	enum spi_baudrate baudrate;
	enum spi_clk_mode clk_mode;
	enum spi_dataframe dataframe;
};

void spi_setup(enum spi_periph periph, struct spi_cfg *cfg);
void spi_enable(enum spi_periph periph);
void spi_disable(enum spi_periph periph);

uint16_t spi_send_halfword(enum spi_periph periph, uint16_t data);
char spi_send_char(enum spi_periph periph, char c);















#endif
