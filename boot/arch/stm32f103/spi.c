#include <stm32f103.h>
#include <spi.h>
#include <stdint.h>
#include <stdbool.h>
#include <cpu.h>

// call this to setup a spi when this spi is disabled
void spi_setup(enum spi_periph periph, struct spi_cfg *cfg){
	uint32_t cr1 = raw_readl((void *)(periph + SPI_CR1));
	uint32_t cr2 = raw_readl((void *)(periph + SPI_CR2));
	if(cr1 & SPI_CR1_SPE){
		// this spi is not disabled, better stop configuring
		return;
	}

	// configure whether this spi is a master or a slave 
	if(cfg->mode == spi_master){
		cr1 |= SPI_CR1_MSTR;
		// if this spi is in master mode, baudrate should be configured
		cr1 &= ~SPI_CR1_BR_MASK;
		cr1 |= SPI_CR1_BR_SHIFT(cfg->baudrate);
	}
	else{
		cr1 &= ~SPI_CR1_MSTR;
	}
	// configure transmit mode
	switch (cfg->trans_mode){
		case bidi_out:
			cr1 |= (SPI_CR1_BIDMODE | SPI_CR1_BIDIOE);
			cr1 &= ~SPI_CR1_RXONLY;
			break;
		case bidi_in:
			cr1 |= SPI_CR1_BIDMODE;
			cr1 &= ~(SPI_CR1_BIDIOE | SPI_CR1_RXONLY);
			break;
		case full_duplex:
			cr1 &= ~(SPI_CR1_BIDMODE | SPI_CR1_RXONLY);
			break;
		case unidi_rxonly:
			cr1 &= ~SPI_CR1_BIDMODE;
			cr1 |= SPI_CR1_RXONLY;
			break;
		default:
	}

	// configure chip select mode (also care about whether this spi is master or slave)
	if(cfg->cs_mode == spi_hardware){
		// hardware chip select
		cr1 &= ~SPI_CR1_SSM;	
		if(cfg->mode == spi_master){
			cr2 |= SPI_CR2_SSOE;	
		}
		else{
			cr2 &= ~SPI_CR2_SSOE;
		}
	}
	else{
		cr1 |= SPI_CR1_SSM;
		if(cfg->mode == spi_master){
			cr1 |= SPI_CR1_SSI;
		}
		else{
			cr1 &= ~SPI_CR1_SSI;
		}

	}

	// configure clock mode (idle high or low, first or second clk level change is considered)
	switch(cfg->clk_mode){
		case idle_high_fst:
			cr1 |= SPI_CR1_CPOL;
			cr1 &= ~SPI_CR1_CPHA;
			break;
		case idle_low_fst:
			cr1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA);
			break;
		case idle_high_snd:
			cr1 |= (SPI_CR1_CPOL | SPI_CR1_CPHA);
			break;
		case idle_low_snd:
			cr1 &= ~SPI_CR1_CPOL;
			cr1 |= SPI_CR1_CPHA;
		default:
	}

	// configure dataframe (8 bit or 16 bit) and lsb/msb
	switch(cfg->dataframe){
		case spi_8l:
			cr1 &= ~SPI_CR1_DFF;
			cr1 |= SPI_CR1_LSBFIRST;
			break;
		case spi_8h:
			cr1 &= ~(SPI_CR1_DFF | SPI_CR1_LSBFIRST);
			break;
		case spi_16l:
			cr1 |= (SPI_CR1_DFF | SPI_CR1_LSBFIRST);
			break;
		case spi_16h:
			cr1 |= SPI_CR1_DFF;
			cr1 &= ~SPI_CR1_LSBFIRST;
			break;
		default:
	}

	raw_writel(cr1, (void *)(periph + SPI_CR1));
	raw_writel(cr2, (void *)(periph + SPI_CR2));
	
	return;
}

static enum spi_mode spi_get_mode(enum spi_periph periph){
	if(raw_readl((void *)(periph + SPI_CR1)) & SPI_CR1_MSTR){
		return spi_master;
	}
	else{
		return spi_slave;
	}
}

static enum spi_trans_mode spi_get_trans_mode(enum spi_periph periph){
	uint32_t cr1 = raw_readl((void *)(periph + SPI_CR1));
	if(cr1 & SPI_CR1_BIDMODE){
		if(cr1 & SPI_CR1_BIDIOE){
			return bidi_out;
		}
		else{
			return bidi_in;
		}
	}
	else{
		if(cr1 & SPI_CR1_RXONLY){
			return unidi_rxonly;
		}
		else{
			return full_duplex;
		}

	}
}

static void spi_wait_tx(enum spi_periph periph){
	while(!(raw_readl((void *)(periph + SPI_SR)) & SPI_SR_TXE)){};
	return;
}

static void spi_wait_rx(enum spi_periph periph){
	while(!(raw_readl((void *)(periph + SPI_SR)) & SPI_SR_RXNE)){}
	return;
}

static void spi_wait_bsy(enum spi_periph periph){
	while(raw_readl((void *)(periph + SPI_SR)) & SPI_SR_BSY){}
	return;
}

void spi_enable(enum spi_periph periph){
	uint32_t cr1 = raw_readl((void *)(periph + SPI_CR1));
	if(cr1 & SPI_CR1_SPE){
		// this spi already enabled
		return;
	}
	cr1 |= SPI_CR1_SPE;
	raw_writel(cr1, (void *)(periph + SPI_CR1));
	while (!(raw_readl((void *)(periph + SPI_CR1)) & SPI_CR1_SPE)){}
	return;
}

// when disable the spi, we just dont care whether the final data is received
void spi_disable(enum spi_periph periph){
	enum spi_mode mode = spi_get_mode(periph);
	enum spi_trans_mode trans_mode = spi_get_trans_mode(periph);
	uint32_t cr1 = raw_readl((void *)(periph + SPI_CR1));
	if(trans_mode == full_duplex){
		spi_wait_tx(periph);
		spi_wait_bsy(periph);
		cr1 &= ~SPI_CR1_SPE;
	}
	else if(trans_mode == bidi_out){
		spi_wait_tx(periph);
		spi_wait_bsy(periph);
		cr1 &= ~SPI_CR1_SPE;
	}
	else if(trans_mode == unidi_rxonly || trans_mode == bidi_in){
		if(mode == spi_master){
			cr1 &= ~SPI_CR1_SPE;
		}
		else{
			cr1 |= SPI_CR1_SPE; // ???
		}
	}
	raw_writel(cr1, (void *)(periph + SPI_CR1));
	return;
}

// use this to send data through spi when dataframe is 16 bit, after sending we can receive 16 bit data from slave
uint16_t spi_send_halfword(enum spi_periph periph, uint16_t data){
	spi_wait_tx(periph);
	raw_writel((uint32_t)data, (void *)(periph + SPI_DR));
	spi_wait_rx(periph);
	return raw_readl((void *)(periph + SPI_DR)) & SPI_DR_MASK;

}
// use this to send data through spi when dataframe is 8 bit, ...
char spi_send_char(enum spi_periph periph, char c){
	return (char)(spi_send_halfword(periph, (uint16_t)c) & 0xff);
}
