#include <rcc.h>
#include <spi.h>
#include <spi_flash.h>
#include <stdint.h>
#include <stdbool.h>

static bool spi_flash_inited = false;
static bool spi_flash_enabled = false;

void spi_flash_init(){
	if(spi_flash_inited){
		return;
	}
	rcc_enable_spi(spi1);
	
	// configure spi1
	struct spi_cfg cfg;
	cfg.mode = spi_master;
	cfg.trans_mode = full_duplex;
	cfg.cs_mode = spi_hardware;
	cfg.baudrate = spi_pclkdiv4;
	cfg.clk_mode = idle_high_snd;
	cfg.dataframe = spi_8h;
	
	spi_setup(spi1, &cfg);

	spi_flash_inited = true;
	return;

}

void spi_flash_enable(){
	if(!spi_flash_inited){
		spi_flash_init();
	}
	if(!spi_flash_enabled){
		spi_enable(spi1);
		spi_flash_enabled = true;
	}
	return;
}

void spi_flash_disable(){
	if(spi_flash_enabled){
		spi_disable(spi1);
		spi_flash_enabled = false;
	}
	return;
}


uint32_t spi_flash_read_jedec(){
	spi_flash_disable();
	spi_flash_enable();
	
	uint32_t jedec_id = 0U;
	spi_send_char(spi1, w25x_jedec_deviceid);
	for(int i = 0;i < 3;i++){
		jedec_id |= (uint32_t)spi_send_char(spi1, dummy_char);
		if(i != 2){
			jedec_id = jedec_id << 8U;
		}
	}
	spi_flash_disable();
	
	return jedec_id;
}


uint32_t spi_flash_read_manufact(){
        spi_flash_disable();
        spi_flash_enable();

        uint32_t manufact_id = 0U;
        spi_send_char(spi1, w25x_manufact_deviceid);
        for(int i = 0;i < 3;i++){
                manufact_id |= (uint32_t)spi_send_char(spi1, dummy_char);
                if(i != 2){
                        manufact_id = manufact_id << 8U;
                }
        }
        spi_flash_disable();

        return manufact_id;
}

