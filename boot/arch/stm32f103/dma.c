#include <stm32f103.h>
#include <dma.h>
#include <stdint.h>
#include <cpu.h>
#include <adc.h>
// call this to setup a channel of a dma device according to cfg
// call this when the channel is disabled
void dma_setup(enum dma_periph periph, enum dma_ch ch, struct dma_ch_cfg *cfg){
	if(periph == dma2 && ch > dma_ch4){
		// dma2 dont have dma_ch5 and dma_ch6
		return;
	}
	
	// fix if there is invalid value in cfg
	if(cfg->msize > 2U){
		cfg->msize = 2U;
	}
	if(cfg->psize > 2U){
		cfg->psize = 2U;
	}
	if(cfg->priority > 3U){
		cfg->priority = 3U;
	}
	if(cfg->ndt > 0xffff){
		cfg->ndt = 0xffff;
	}

	// set ndt (transferring times)
	uint32_t cndtr = raw_readl((void *)(periph + DMA_CNDTR(ch)));
	cndtr &= ~DMA_CNDTR_NDT_MASK;
	cndtr |= DMA_CNDTR_NDT_SHIFT(cfg->ndt);
	raw_writel(cndtr, (void *)(periph + DMA_CNDTR(ch)));

	// set periph addr
	raw_writel(cfg->pa, (void *)(periph + DMA_CPAR(ch)));

	// set memory addr
	raw_writel(cfg->ma, (void *)(periph + DMA_CMAR(ch)));

	// set msize 
	uint32_t ccr = raw_readl((void *)(periph + DMA_CCR(ch)));
	ccr &= ~(DMA_CCR_MSIZE_MASK);
	ccr |= DMA_CCR_MSIZE_SHIFT((uint32_t)(cfg->msize));

	// set psize
	ccr &= ~(DMA_CCR_PSIZE_MASK);
	ccr |= DMA_CCR_PSIZE_SHIFT((uint32_t)(cfg->psize));

	// set priority for this channel
	ccr &= ~(DMA_CCR_PL_MASK);
	ccr |= DMA_CCR_PL_SHIFT((uint32_t)(cfg->priority));

	if(cfg->m2m){
		ccr |= DMA_CCR_MEM2MEM;		
	}
	else{
		ccr &= ~DMA_CCR_MEM2MEM;
	}
	// set direction
	if(cfg->direction){
		// read from mem, write to periph
		ccr |= DMA_CCR_DIR;
	}
	else{
		// read from periph, write to mem
		ccr &= ~DMA_CCR_DIR;
	}

	if(cfg->minc){
		ccr |= DMA_CCR_MINC;
	}
	else{
		ccr &= ~DMA_CCR_MINC;
	}

	if(cfg->pinc){
		ccr |= DMA_CCR_PINC;
	}
	else{
		ccr &= ~DMA_CCR_PINC;
	}

	if(cfg->circular){
		ccr |= DMA_CCR_CIRC;
	}
	else{
		ccr &= ~DMA_CCR_CIRC;
	}

	if(cfg->teie){
		ccr |= DMA_CCR_TEIE;
	}
	else{
		ccr &= ~DMA_CCR_TEIE;
	}

	if(cfg->htie){
		ccr |= DMA_CCR_HTIE;
	}
	else{
		ccr &= ~DMA_CCR_HTIE;
	}
	
	if(cfg->tcie){
		ccr |= DMA_CCR_TCIE;
	}
	else{
		ccr &= ~DMA_CCR_TCIE;
	}
	
	raw_writel(ccr, (void *)(periph + DMA_CCR(ch)));
	
	
}

void dma_enable_ch(enum dma_periph periph, enum dma_ch ch){
	if(periph == dma2 && ch > dma_ch4){
		return;
	}
	uint32_t ccr = raw_readl((void *)(periph + DMA_CCR(ch)));
	if(ccr & DMA_CCR_EN){
		//this channel already enabled
		return;
	}
	else{
		ccr |= DMA_CCR_EN;
		raw_writel(ccr, (void *)(periph + DMA_CCR(ch)));
	}

	return;
}

void dma_disable_ch(enum dma_periph periph, enum dma_ch ch){
	if(periph == dma2 && ch > dma_ch4){
                return;
        }
        uint32_t ccr = raw_readl((void *)(periph + DMA_CCR(ch)));
        if(!(ccr & DMA_CCR_EN)){
                //this channel already disabled
                return;
        }
        else{
                ccr &= ~DMA_CCR_EN;
                raw_writel(ccr, (void *)(periph + DMA_CCR(ch)));
        }

        return;



}

uint32_t dma_buffer[0xf];

void init_dma(){
	struct dma_ch_cfg ch_cfg;
	ch_cfg.m2m = false;		// p to m
	ch_cfg.direction = false;	// read from periph
	ch_cfg.minc = true;
	ch_cfg.pinc = false;
	ch_cfg.circular = true;
	ch_cfg.teie = false;
	ch_cfg.htie = false;
	ch_cfg.tcie = false;
	ch_cfg.psize = 2U;		// 32 bit	
	ch_cfg.msize = 2U;
	ch_cfg.priority = 3U;		// very high
	ch_cfg.ndt = 0xf;
	ch_cfg.pa = ADC1_BASE + ADC_DR;
	ch_cfg.ma = (uint32_t)dma_buffer;

	dma_setup(dma1, dma_ch0, &ch_cfg);
	dma_enable_ch(dma1, dma_ch0);
	
	return;
}









