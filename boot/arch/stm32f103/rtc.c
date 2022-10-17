#include <stm32f103.h>
#include <rtc.h>
#include <rcc.h>
#include <stdint.h>
#include <cpu.h>
#include <pwr.h>
#include <stdbool.h>

// rtc regs need to be read after RTC_CRL_RSF bit is set to ensure value we get is correct
uint32_t read_rtc_reg(uint32_t rtc_reg_offset){
        uint32_t crl;
        // check whether the offset is within rtc regs and valid
        if(rtc_reg_offset > 0x24 || rtc_reg_offset & 0x3){
                return 0;
        }

        // wait until RTC_CRL_RSF is set
        do{
                crl = raw_readl((void *)(RTC_BASE + RTC_CRL));
        }while(!(crl & RTC_CRL_RSF));

        // return the reg value required
        return raw_readl((void *)(RTC_BASE + rtc_reg_offset));
}

// writing to rtc regs has pre-conditions, using this function to to ensure safe writting
void write_rtc_reg(uint32_t rtc_reg_offset, uint32_t val){
	uint32_t crl;
	// check whether the offset is within rtc regs and valid
	if(rtc_reg_offset > 0x24 || rtc_reg_offset & 0x3){
		return;
	}
	// wait until the RTOFF turns to 1
	do{
		crl = read_rtc_reg(RTC_CRL);
	}while(!(crl & RTC_CRL_RTOFF));
	
	// enter configure mode
	crl |= RTC_CRL_CNF;
	raw_writel(crl, (void*)(RTC_BASE + RTC_CRL));
	
	if(rtc_reg_offset == RTC_CRL){
		/* 
		 * the reg to be written is just CRL itself.
		 *
		 * we'd better keep CNF bit to stay in configure mode, because
		 * don't know whether it is permitted to write val to CRL and exit configure mode at the same time.
		 *
		 * the RTOFF seems dont need to keep since it is read-only
		 * but we just keep it...
		 */
		val |= RTC_CRL_CNF | RTC_CRL_RTOFF;
		// if we writing val to CRL, we need to sync crl according to val, else crl will be covered when exiting configure mode
		crl = val;
	}
	// write val to reg addr(except CRL)
	raw_writel(val, (void *)(RTC_BASE + rtc_reg_offset));

	// exit the configure mode
	// if the reg to be written is just CRL, the crl bits are already set according to val 
	crl &= ~RTC_CRL_CNF;
	raw_writel(crl, (void *)(RTC_BASE + RTC_CRL));
	
	// wait until the configure is terminated
	do{
                crl = read_rtc_reg(RTC_CRL);
        }while(!(crl & RTC_CRL_RTOFF));

	return;
}



void init_rtc(){
	
	uint32_t checkr = raw_readl((void *)(BKP_BASE + RTC_CHECKR));
	if(checkr == RTC_CHECK_VAL){
		// bkp not reset
		return;
	}
	else{
		// set bkp registers as writable		
		bkp_write_protect(false);
		// set rcc bdcr reg to enable rtc with LSE oscillator
		enable_rtc();

		// configure rtc regs (seems neet to enter and exit configure mode several times, need to be optimized)
		// 1: set default prescale
		write_rtc_reg(RTC_PRLH, RTC_PRLH_VAL);
		write_rtc_reg(RTC_PRLL, RTC_PRLL_VAL);

		// 2: set default count
		write_rtc_reg(RTC_CNTH, RTC_CNTH_VAL);
		write_rtc_reg(RTC_CNTL, RTC_CNTL_VAL);

		// mark rtc as inited
		checkr = RTC_CHECK_VAL;
		raw_writel(checkr, (void *)(BKP_BASE + RTC_CHECKR));

		// protect bkp from being written
		bkp_write_protect(true);
	}

	return;
}

uint32_t rtc_read_cnt(){
	uint32_t cnth = read_rtc_reg(RTC_CNTH);
	uint32_t cntl = read_rtc_reg(RTC_CNTL);

	return (cnth << 16) | cntl;
}









