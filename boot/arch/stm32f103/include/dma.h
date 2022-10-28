#ifndef __DMA_H
#define __DMA_H

#include <stm32f103.h>
#include <stdint.h>
#include <stdbool.h>

#define DMA_ISR				(0x00)
#define DMA_ISR_TEIF(x)			(1U << (3 + ((x) << 2)))  // (transfer error) x = 0..6 (reference manual 1..7)  DMA2 only 0..4
#define DMA_ISR_HTIF(x)			(1U << (2 + ((x) << 2)))  // half transfer event occured
#define DMA_ISR_TCIF(x)			(1U << (1 + ((x) << 2)))  // transfer complete event occured
#define DMA_ISR_GIF(x)			(1U << (0 + ((x) << 2)))  // one or more of the event above occured

#define DMA_IFCR			(0x04)
#define DMA_IFCR_CTEIF(x)		(1U << (3 + ((x) << 2)))  // set 1 to clear TEIF
#define DMA_IFCR_CHTIF(x)		(1U << (2 + ((x) << 2)))  // set 1 to clear HTIF
#define DMA_IFCR_CTCIF(x)		(1U << (1 + ((x) << 2)))  // set 1 to clear TCIF
#define DMA_IFCR_CGIF(x)		(1U << (0 + ((x) << 2)))  // set 1 to clear GIF


#define DMA_CCR(x)			(0x08 + ((x) * 0x14))
#define DMA_CCR_MEM2MEM			(1U << 14)	// mem to mem mode
#define DMA_CCR_PL_SHIFT(x)		((x) << 12)	// 2 bits  (priority 00:Low, 11:Very high)
#define DMA_CCR_PL_MASK			(DMA_CCR_PL_SHIFT(3U))
#define DMA_CCR_MSIZE_SHIFT(x)		((x) << 10)	// 2 bits  (memory data size, 00: 8bit, 01: 16bit, 10: 32bit, 11: reserved)
#define DMA_CCR_MSIZE_MASK		(DMA_CCR_MSIZE_SHIFT(3U))
#define DMA_CCR_PSIZE_SHIFT(x)		((x) << 8)	// 2 bits  (periph data size, 00: 8bit, 01: 16bit, 10: 32bit, 11: reserved)
#define DMA_CCR_PSIZE_MASK		(DMA_CCR_PSIZE_SHIFT(3U))
#define DMA_CCR_MINC			(1U << 7)	// set 1 to enable memory increment mode
#define DMA_CCR_PINC			(1U << 6)	// ...		   periph ...
#define DMA_CCR_CIRC			(1U << 5)	// circular mode
#define DMA_CCR_DIR			(1U << 4)	// direction. 0: p to m, 1: m to p
#define DMA_CCR_TEIE			(1U << 3)	// transfer error interrupt
#define DMA_CCR_HTIE			(1U << 2)	// transfer half interrupt
#define DMA_CCR_TCIE			(1U << 1)	// transfer complete interrupt
#define DMA_CCR_EN			(1U << 0)	// enable this channel


#define DMA_CNDTR(x)			(0x0c + ((x) * 0x14))
#define DMA_CNDTR_NDT_SHIFT(x)		((x) << 0)	// set number of data to transfer
#define DMA_CNDTR_NDT_MASK		(DMA_CNDTR_NDT_SHIFT(0xffff))


#define DMA_CPAR(x)			(0x10 + ((x) * 0x14))
#define DMA_CPAR_PA_SHIFT(x)		((x) << 0)
#define DMA_CPAR_PA_MASK		(DMA_CPAR_PA_SHIFT(0xffffffff))


#define DMA_CMAR(x)                     (0x14 + ((x) * 0x14))
#define DMA_CMAR_MA_SHIFT(x)            ((x) << 0)
#define DMA_CMAR_MA_MASK                (DMA_CMAR_MA_SHIFT(0xffffffff))



enum dma_periph {dma1 = DMA1_BASE, dma2 = DMA2_BASE};

// dma2 only have dma_ch{0..4}
enum dma_ch {dma_ch0 = 0, dma_ch1, dma_ch2, dma_ch3, dma_ch4, dma_ch5, dma_ch6};

struct dma_ch_cfg {
	bool m2m  	: 1;
	bool direction	: 1;
	bool minc 	: 1;
	bool pinc 	: 1;
	bool circular	: 1;
	bool teie	: 1;
	bool htie	: 1;
	bool tcie	: 1;
	uint8_t psize;
	uint8_t msize;
	uint8_t priority;
	uint32_t ndt;
	uint32_t pa;
	uint32_t ma;
};
void dma_setup(enum dma_periph periph, enum dma_ch ch, struct dma_ch_cfg *cfg);
void dma_enable_ch(enum dma_periph periph, enum dma_ch ch);
void dma_disable_ch(enum dma_periph periph, enum dma_ch ch);


void init_dma();


#endif
