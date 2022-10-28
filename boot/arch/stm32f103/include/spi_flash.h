#ifndef	__SPI_FLASH_H
#define __SPI_FLASH_H

#include <stdint.h>

#define dummy_char		((char)0xff)

// commands for w25x flash
#define w25x_write_enable	((char)0x06)
#define w25x_write_disable	((char)0x04)
#define w25x_read_status	((char)0x05)
#define w25x_status_busy	(1U << 0)

#define w25x_write_status	((char)0x01)

#define w25x_read_data		((char)0x03)
#define w25x_fast_read_data	((char)0x0b)
#define w25x_fast_read_dual	((char)0x3b)

#define w25x_page_program	((char)0x02)
#define w25x_block_erase	((char)0xd8)
#define w25x_sector_erase	((char)0x20)
#define w25x_chip_erase		((char)0xc7)

#define w25x_powerdown		((char)0xb9)
#define w25x_release_powerdown	((char)0xab)

#define w25x_manufact_deviceid	((char)0x90)
#define w25x_jedec_deviceid	((char)0x9f)


#define w25x_sector_high(addr)		((char)(((addr) & 0xff0000) >> 16))
#define w25x_sector_low(addr)		((char)(((addr) & 0xff00) >> 8))
#define w25x_sector_offset(addr)	((char)((addr) & 0xff))


#define W25X_CAPACITY		(0x800000)
#define W25X_MAX_ADDR		(W25X_CAPACITY - 1)

#define W25X_PAGE_SIZE		(0x100)
#define W25X_SECTOR_SIZE	(W25X_PAGE_SIZE << 4)
#define W25X_BLOCK_SIZE		(W25X_SECTOR_SIZE << 4)

#define W25X_PAGE_OFFSET_MASK   	(W25X_PAGE_SIZE - 1)
#define W25X_SECTOR_OFFSET_MASK   	(W25X_SECTOR_SIZE - 1)
#define W25X_BLOCK_OFFSET_MASK   	(W25X_BLOCK_SIZE - 1)

#define W25X_PAGE_MASK          (~(W25X_PAGE_OFFSET_MASK))
#define W25X_SECTOR_MASK        (~(W25X_SECTOR_OFFSET_MASK))
#define W25X_BLOCK_MASK         (~(W25X_BLOCK_OFFSET_MASK))

#define W25X_PAGE_OFFSET(addr)           ((addr) & W25X_PAGE_OFFSET_MASK)
#define W25X_SECTOR_OFFSET(addr)         ((addr) & W25X_SECTOR_OFFSET_MASK)
#define W25X_BLOCK_OFFSET(addr)          ((addr) & W25X_BLOCK_OFFSET_MASK)


#define W25X_PAGE_ALIGN(addr)	((addr) & W25X_PAGE_MASK)
#define W25X_SECTOR_ALIGN(addr)	((addr) & W25X_SECTOR_MASK)
#define W25X_BLOCK_ALIGN(addr)	((addr) & W25X_BLOCK_MASK)

#define W25X_VALID_ADDR(addr)	( 0U <= (addr) && (addr) <= W25X_MAX_ADDR )







void spi_flash_enable();
void spi_flash_disable();

uint32_t spi_flash_read_jedec();
uint32_t spi_flash_read_manufact();

void spi_flash_write_protect(bool prot);
void spi_flash_chip_erase();
void spi_flash_erase_sector(uint32_t addr);
void spi_flash_write(uint32_t addr, char *buf, uint32_t sz, bool erase);
void spi_flash_read(uint32_t addr, char *buf, uint32_t sz);
#endif
