#include <rcc.h>
#include <spi.h>
#include <spi_flash.h>
#include <stdint.h>
#include <stdbool.h>
#include <mm/mman.h>


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

void spi_flash_reconnect(){
	spi_flash_disable();
	spi_flash_enable();
	return;
}


void spi_flash_powerdown(){
        spi_flash_reconnect();
	spi_send_char(spi1, w25x_powerdown);
	spi_flash_disable();
}

void spi_flash_release_powerdown(){
        spi_flash_reconnect();
        spi_send_char(spi1, w25x_release_powerdown);
        spi_flash_disable();
}



uint32_t spi_flash_read_jedec(){
	spi_flash_reconnect();
	
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


void spi_flash_write_protect(bool prot){
	spi_flash_reconnect();
	if(prot){
		spi_send_char(spi1, w25x_write_disable);
	}
	else{
		spi_send_char(spi1, w25x_write_enable);
	}
	spi_flash_disable();
	return;
}

void spi_flash_wait_busy(){
	uint8_t status;
	spi_flash_reconnect();
	spi_send_char(spi1, w25x_read_status);
	do{
		status = (uint8_t)spi_send_char(spi1, dummy_char);

	}while(status & w25x_status_busy);
	spi_flash_disable();

	return;
}

void spi_flash_chip_erase(){
        spi_flash_write_protect(false);

	spi_flash_reconnect();
	spi_send_char(spi1, w25x_chip_erase);
	spi_flash_disable();
	// wait until spi_flash finishes chip erasing
	spi_flash_wait_busy();
	spi_flash_write_protect(true);
	return;
}


void spi_flash_erase_sector(uint32_t addr){
	if(W25X_SECTOR_OFFSET(addr)){
		// the addr is not aligned by sector, refuse erasing
		return;
	}

	spi_flash_write_protect(false);
	spi_flash_reconnect();
	spi_send_char(spi1, w25x_sector_erase);
	spi_send_char(spi1, w25x_sector_high(addr));
	spi_send_char(spi1, w25x_sector_low(addr));
	spi_send_char(spi1, w25x_sector_offset(addr));

	spi_flash_disable();
	// wait until spi_flash finishes erasing
	spi_flash_wait_busy();
	spi_flash_write_protect(true);

	return;
}


/* 
 * this function write buf to spi_flash addr
 * unaligned addr is also accepted
 * after chip erasing or sectors involved are erased before, 'erase' can set as false
 */
void spi_flash_write(uint32_t addr, char *buf, uint32_t sz, bool erase){
	if((! W25X_VALID_ADDR(addr)) || sz == 0){
		return;
	}
	
	uint32_t start = addr; uint32_t end = addr + sz;
	if(end > W25X_MAX_ADDR + 1){
		end = W25X_MAX_ADDR + 1;
	}
	uint32_t align_start = W25X_SECTOR_ALIGN(start);
	uint32_t align_end = 0;
	if(W25X_SECTOR_ALIGN(end) == end){
		align_end = end;
	}
	else{
		align_end = W25X_SECTOR_ALIGN(end) + W25X_SECTOR_SIZE;
	}
	char *front = NULL; char *real = NULL;
	if(align_start != start){
		// we need to preserve data in the first section before 'start'
		front = mem_alloc(start - align_start);
		if(front == NULL){
			return;
		}
	}
	if(align_end != end){
		// we need to preserve data in the last section starting at 'end'
		real = mem_alloc(align_end - end);
		if(real == NULL){
			return;
		}
	}
	if(front != NULL){
		spi_flash_read(align_start, front, start - align_start);

	}
	if(real != NULL){
		spi_flash_read(end, real, align_end - end);
	}
		
	uint32_t w_addr = align_start;
	while(w_addr < align_end){
		if(!(W25X_SECTOR_OFFSET(w_addr))){
			// arrive starting of a sector	
			if(erase){
				// erase this sector for writting
				spi_flash_erase_sector(w_addr);
			}
		}
		if(!(W25X_PAGE_OFFSET(w_addr))){
			// arrive starting of a page
			// need to enter a new 'page programming'
			spi_flash_wait_busy();
			spi_flash_write_protect(false);
			spi_flash_reconnect();
			spi_send_char(spi1, w25x_page_program);
			spi_send_char(spi1, w25x_sector_high(w_addr));
			spi_send_char(spi1, w25x_sector_low(w_addr));
			spi_send_char(spi1, w25x_sector_offset(w_addr));
		}

		if(w_addr < start){
			spi_send_char(spi1, *(front + w_addr - align_start));
		}
		else if(w_addr < end){
			spi_send_char(spi1, *(buf + w_addr - start));
		}
		else{
			spi_send_char(spi1, *(real + w_addr - end));
		}
		w_addr++;
	}
	if(front != NULL){
		mem_free(front);
	}
	if(real != NULL){
		mem_free(real);
	}

	spi_flash_wait_busy();
	spi_flash_write_protect(true);
	
	return;
}


void spi_flash_read(uint32_t addr, char *buf, uint32_t sz){
	if((!(W25X_VALID_ADDR(addr))) || sz == 0){
		// wrong arguments
		return;
	}
	if(addr + sz > W25X_CAPACITY){
		// we just read till the end of flash address region
		sz = W25X_CAPACITY - addr;
	}

	spi_flash_reconnect();
	spi_send_char(spi1, w25x_read_data);
	spi_send_char(spi1, w25x_sector_high(addr));
	spi_send_char(spi1, w25x_sector_low(addr));
	spi_send_char(spi1, w25x_sector_offset(addr));
	
	while(sz--){
		*buf++ = spi_send_char(spi1, dummy_char);
	}

	spi_flash_disable();

	return;
}




















