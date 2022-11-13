#include <stm32f103.h>
#include <cpu.h>
#include <usart.h>

static struct usart_t serial;

// using this function to calculate USART_BRR value, but it seems wrong.
static uint32_t calculate_brr(uint32_t baud_rate, uint32_t base_clk, uint32_t oversampling){
	uint32_t tmp_res = (base_clk * 100) / (baud_rate * oversampling);
	uint32_t tmp_int = tmp_res / 100;
	uint32_t tmp_frac = (((tmp_res % 100) * oversampling) + 50) / 100;
	
	return ((tmp_int << 4) | tmp_frac);
}

static void wait_tx_ready(struct usart_t *dev){
	while( !(raw_readl((void *)(dev->base_addr + USART_SR)) & USART_SR_TXE) ){};
	return;
}

static void wait_rx_ready(struct usart_t *dev){
	while( !(raw_readl((void *)(dev->base_addr + USART_SR)) & USART_SR_RXNE) ){};
	return;
}

static void usart_setup(struct usart_t *dev){
	dev->base_addr = USART1_BASE;
	dev->baud_rate = BAUD_RATE;
	dev->data_bits = DATA_BITS;
	dev->stop_bits = STOP_BITS;
	dev->oversampling = OVERSAMPLING;
	return;
}

static void usart_reset(struct usart_t *dev){
	// first, we disable usart before configuring
	uint32_t cr1 = raw_readl((void *)(dev->base_addr + USART_CR1));
	cr1 &= ~USART_CR1_UE;
	raw_writel(cr1, (void *)(dev->base_addr + USART_CR1));


	// set baud_rate register
	uint32_t brr = calculate_brr(dev->baud_rate, PCLK2, dev->oversampling);
	// result from 'calculate_brr' seems is wrong. temporarily writing hard code value.
	brr = 0x00000271;
	raw_writel(brr, (void *)(dev->base_addr + USART_BRR)); //??? why my calculation is wrong?


	// set cr2
	if(dev->stop_bits == 1){
		raw_writel(0, (void *)(dev->base_addr + USART_CR2));
	}
	else{
		//to be implemented
	}


	//set cr3
	raw_writel(0, (void *)(dev->base_addr + USART_CR3));


	//set gtpr
	raw_writel(0, (void *)(dev->base_addr + USART_GTPR));


	//set cr1
	cr1 = 0;
	// if oversampling is 8, we need to set the over8 bit of cr1
	if(dev->oversampling == 8){
		cr1 |= USART_OVER8;
	}
	// set the data_bits of cr1	
	if(dev->data_bits != 8){
		cr1 |= USART_CR1_M;
	}
	// enable TX and RX
	cr1 |= (USART_CR1_TE | USART_CR1_RE);
	raw_writel(cr1, (void *)(dev->base_addr + USART_CR1));
	// enable usart
	cr1 |= USART_CR1_UE;
	raw_writel(cr1, (void *)(dev->base_addr + USART_CR1));
	
	return;
}

void init_usart(){
	usart_setup(&serial);
	usart_reset(&serial);

	return;
}

void usart_send_char(char c, struct usart_t *dev){
	wait_tx_ready(dev);
	raw_writel((uint32_t)c, (void *)(dev->base_addr + USART_DR));
	
	return;
}

char usart_get_char(struct usart_t *dev){
	uint32_t tmp;
	wait_rx_ready(dev);
	tmp = raw_readl((void *)(dev->base_addr + USART_DR));
	
	return (char)(tmp & USART_DR_DATA_MASK);
}


char getc(){
        return usart_get_char(&serial);
}

void putc(char c){
        usart_send_char(c, &serial);

	return;
}
















