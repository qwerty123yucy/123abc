#include <stm32f103.h>
#include <adc.h>
#include <cpu.h>
#include <stdint.h>
#include <rcc.h>

void adc_set_awdg(enum adc_periph periph, struct awdg_cfg *cfg){
	uint32_t cr1;

	// first disable awdg
	cr1 = raw_readl((void *)(periph + ADC_CR1));
	cr1 &= ~ADC_CR1_AWDEN;
	cr1 &= ~ADC_CR1_JAWDEN;
	raw_writel(cr1, (void *)(periph + ADC_CR1));


	// fix if the high and low value of awdg cfg is not valid
	if(cfg->high <= cfg->low){
		cfg->high = ADC_HLTR_MAX;
		cfg->low = 0U;
	}
	else if(cfg->high > ADC_HLTR_MAX){
		cfg->high = ADC_HLTR_MAX;
		if(cfg->low >= ADC_HLTR_MAX){
			cfg->low = 0U;
		}
	}

	// write high low value to registers
	raw_writel(cfg->high, (void *)(periph + ADC_HTR));
	raw_writel(cfg->low, (void *)(periph + ADC_LTR));

	// configure cr1
	if(cfg->is_single){
		// awdg on single channel
		cr1 |= ADC_CR1_AWDSGL;
		cr1 &= ~ADC_CR1_AWDCH_MASK;
		cr1 |= ADC_CR1_AWDCH_SHIFT(cfg->ch);
	}
	else{
		// awdg on all channel
		cr1 &= ~(ADC_CR1_AWDSGL);
	}

	if(cfg->enable_interrupt){
		// awdg interrupt enable
		cr1 |= ADC_CR1_AWDIE;
	}
	else{
		// awdg interrupt disable
		cr1 &= ~ADC_CR1_AWDIE;
	}
	
	// enable awdg according to cfg
	if(cfg->enable_regular){
		cr1 |= ADC_CR1_AWDEN;	
	}
	if(cfg->enable_inject){
		cr1 |= ADC_CR1_JAWDEN;
	}

	// write cr1
	raw_writel(cr1, (void *)(periph + ADC_CR1));
	
	return;
}

// this function set adc according to the num of regular channel and the channels
// length of ch_lst must be equal to or bigger than ch_num
void adc_set_rchannel(enum adc_periph periph, uint32_t ch_num, enum adc_ch *ch_lst){
	uint32_t num;
	// check whether ch_num is valid
	if(periph == adc3){
		// adc3 only support 8 regular channels
		num = (ch_num > ADC3_SQR_MAX + 1 ? ADC3_SQR_MAX + 1 : ch_num);
	}
	else{
		// adc1 adc2 support 16 channels
		num = (ch_num > ADC_SQR_MAX + 1 ? ADC_SQR_MAX + 1 : ch_num);
	}

	uint32_t sqr_ptr = ADC_SQR(0U);
	uint32_t sqr = raw_readl((void *)(periph + sqr_ptr));
	
	for(int i = 0;i < num;i++){
		// each sqr contains 6 channels
		if(i != 0 && !(i % 6)){
			raw_writel(sqr, (void *)(periph + sqr_ptr));
			sqr_ptr--;
			sqr = raw_readl((void *)(periph + sqr_ptr));

		}
		sqr &= ~ADC_SQR_MASK(i);
		sqr |= ADC_SQR_SHIFT(i, ch_lst[i]);
	}
	raw_writel(sqr, (void *)(periph + sqr_ptr));
	
	// write num of channels to ADC_SQR_L bits
	sqr = raw_readl((void *)(periph + ADC_SQR(ADC_SQR_MAX)));
	sqr &= ~ADC_SQR_L_MASK;
	sqr |= ADC_SQR_L_SHIFT(num);

	return;
}

// set channel ch's sample cycle
void adc_set_sample(enum adc_periph periph, enum adc_ch ch, enum adc_sample smp){
	uint32_t smpr = raw_readl((void *)(periph + ADC_SMPR(ch)));
	
	smpr &= ~ADC_SMPR_MASK(ch);
	smpr |= ADC_SMPR_SHIFT(ch, smp);

	raw_writel(smpr, (void *)(periph + ADC_SMPR(ch)));

	return;
}


// better disable adc before setting mode. 
// after setting mode, manually starting adc is needed.
void adc_set_mode(enum adc_periph periph, struct adc_mode_cfg *cfg){
	uint32_t cr1 = raw_readl((void *)(periph + ADC_CR1));
	uint32_t cr2 = raw_readl((void *)(periph + ADC_CR2));
	
	switch(cfg->mode){
		case single:
			cr1 &= ~ADC_CR1_SCAN;
			cr1 &= ~ADC_CR1_DISCEN;
			cr2 &= ~ADC_CR2_CONT;
			break;
		case continous:
			cr1 &= ~ADC_CR1_SCAN;
			cr1 &= ~ADC_CR1_DISCEN;
			cr2 |= ADC_CR2_CONT;
			break;
		case scan:
			cr1 |= ADC_CR1_SCAN;
			cr1 &= ~ADC_CR1_DISCEN;
			cr2 &= ~ADC_CR2_CONT;
			break;
		case continous_scan:
			cr1 |= ADC_CR1_SCAN;
			cr1 &= ~ADC_CR1_DISCEN;
			cr2 |= ADC_CR2_CONT;
			break;
		case discontinous:
			cr1 &= ~ADC_CR1_SCAN;
			cr2 &= ~ADC_CR2_CONT;
			cr1 &= ~ADC_CR1_DISCNUM_MASK;
			if(cfg->disc_num > ADC_CR1_DISCNUM_MAX){
				cfg->disc_num = ADC_CR1_DISCNUM_MAX;
			}
			cr1 |= ADC_CR1_DISCNUM_SHIFT(cfg->disc_num);
			cr1 |= ADC_CR1_DISCEN;
			break;
		default:
			break;
	}

	raw_writel(cr1, (void *)(periph + ADC_CR1));
	raw_writel(cr2, (void *)(periph + ADC_CR2));
	
	return;
}

// wake up adc from power down state
void adc_wakeup(enum adc_periph periph){
	uint32_t cr2 = raw_readl((void *)(periph + ADC_CR2));
	if(cr2 & ADC_CR2_ADON){
		return;
	}
	cr2 |= ADC_CR2_ADON;
	raw_writel(cr2, (void *)(periph + ADC_CR2));

	// wait until adc is really on
	while(!(raw_readl((void *)(periph + ADC_CR2)) & ADC_CR2_ADON)){}
	
	return;
}

// start conversion
// if the adc is in power down state, this function automatically wake up it
void adc_start(enum adc_periph periph){
	uint32_t cr2 = raw_readl((void *)(periph + ADC_CR2));
	if(! (cr2 & ADC_CR2_ADON)){
		// adc is powered down
		adc_wakeup(periph);
		cr2 |= ADC_CR2_ADON;
	}
	
	// start conversion
	raw_writel(cr2, (void *)(periph + ADC_CR2));

	return;
}

// power down the adc (this function will wait until it really powered down)
void adc_power_down(enum adc_periph periph){
	uint32_t cr2 = raw_readl((void *)(periph + ADC_CR2));
	if(!(cr2 & ADC_CR2_ADON)){
		return;
	}
	cr2 &= ~ADC_CR2_ADON;
	raw_writel(cr2, (void *)(periph + ADC_CR2));
	while(raw_readl((void *)(periph + ADC_CR2)) & ADC_CR2_ADON){};

	return;
}

uint32_t adc_get_rdata(enum adc_periph periph){
	// wait until end of conversion
	while( !(raw_readl((void *)(periph + ADC_SR)) & ADC_SR_EOC) ){}

	return raw_readl((void *)(periph + ADC_DR)) & ADC_DR_DATA_MASK;
}


// better call this when adc is off
void adc_enable_dma(enum adc_periph periph){
	if(periph == adc2){
		return;
	}
	uint32_t cr2 = raw_readl((void *)(periph + ADC_CR2));
	if(cr2 & ADC_CR2_DMA){
		return;
	}
	cr2 |= ADC_CR2_DMA;
	raw_writel(cr2, (void *)(periph + ADC_CR2));
	
	return;
}

// better call this when adc is off
void adc_disable_dma(enum adc_periph periph){
        if(periph == adc2){
                return;
        }
        uint32_t cr2 = raw_readl((void *)(periph + ADC_CR2));
        if(!(cr2 & ADC_CR2_DMA)){
                return;
        }
        cr2 &= ~ADC_CR2_DMA;
        raw_writel(cr2, (void *)(periph + ADC_CR2));

        return;
}



double adc_get_temp(){
	adc_disable_dma(adc1);
	
	uint32_t adc_sample = 0;
	for(int i = 0;i < 5;i++){
		adc_sample += adc_get_rdata(adc1);
	}
	adc_sample /= 5;
	double V_SENSOR = (adc_sample * (V_REFINT / ADC_HLTR_MAX)) ;
	double temperature = (V_25 - V_SENSOR) / V_PERC + 25l; 
	
	adc_enable_dma(adc1);
	return temperature;
}




void init_adc(){

	// a demo continously convert the builtin temperature sensor
	
	rcc_enable_adc(adc1, adc_div6);
	
	struct awdg_cfg awdg;
	awdg.enable_regular = false;
	awdg.enable_inject = false;
	awdg.is_single = false;
	awdg.enable_interrupt = false;
	awdg.low = 0U; awdg.high = ADC_HLTR_MAX;
	awdg.ch = adc_ch0;

	adc_set_awdg(adc1, &awdg);

	struct adc_mode_cfg mode_cfg;
	mode_cfg.mode = continous;
	mode_cfg.disc_num = 0U;

	adc_set_mode(adc1, &mode_cfg);

	enum adc_ch ch_lst[1] = { adc_ch16 };
	adc_set_rchannel(adc1, 1U, ch_lst);

	adc_set_sample(adc1, adc_ch16, c239p5);

	uint32_t cr2 = raw_readl((void *)(adc1 + ADC_CR2));
	cr2 |= ADC_CR2_TSVREFE;
	raw_writel(cr2, (void *)(adc1 + ADC_CR2));

	adc_enable_dma(adc1);

	adc_start(adc1);

	return;
}















































