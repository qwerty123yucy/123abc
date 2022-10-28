#ifndef	__SPI_FLASH_H
#define __SPI_FLASH_H

#include <stdint.h>

#define dummy_char		((char)0xff)

// commands for w25x flash
#define w25x_write_enable	(0x06)
#define w25x_write_disable	(0x04)
#define w25x_read_status	(0x05)
#define w25x_write_status	(0x01)

#define w25x_read_data		(0x03)
#define w25x_fast_read_data	(0x0b)
#define w25x_fast_read_dual	(0x3b)

#define w25x_page_program	(0x02)
#define w25x_block_erase	(0xd8)
#define w25x_sector_erase	(0x20)
#define w25x_chip_erase		(0xc7)

#define w25x_powerdown		(0xb9)
#define w25x_release_powerdown	(0xab)

#define w25x_manufact_deviceid	(0x90)
#define w25x_jedec_deviceid	(0x9f)

#endif


void spi_flash_enable();
void spi_flash_disable();

uint32_t spi_flash_read_jedec();
uint32_t spi_flash_read_manufact();
