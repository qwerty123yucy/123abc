/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include <spi_flash.h>
#include <utils/time.h>
#include <rtc.h>
#include <stdbool.h>

/* Definitions of physical drive number for each drive */
#define DEV_SPI_FLASH	0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */
#define DEV_RAM		3	// (modify) spi_flash as dev 3

#define SPI_FLASH_JEDEC (0x00ef4017)

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS SPI_FLASH_disk_status(){
        // read spi_flash_jedec, if result is as expected, return 0(status ok);
        if(spi_flash_read_jedec() == SPI_FLASH_JEDEC){
                return STA_NOINIT & 0x00;
        }
        return STA_NOINIT;
}

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;
	// int result;

	switch (pdrv) {
	case DEV_RAM :
		// result = RAM_disk_status();

		// translate the reslut code here

		return stat;

	case DEV_MMC :
		// result = MMC_disk_status();

		// translate the reslut code here

		return stat;

	case DEV_USB :
		// result = USB_disk_status();

		// translate the reslut code here

		return stat;
	
	case DEV_SPI_FLASH:
		
		return SPI_FLASH_disk_status();
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;
	// int result;

	switch (pdrv) {
	case DEV_RAM :
		// result = RAM_disk_initialize();

		// translate the reslut code here

		return stat;

	case DEV_MMC :
		// result = MMC_disk_initialize();

		// translate the reslut code here

		return stat;

	case DEV_USB :
		// result = USB_disk_initialize();

		// translate the reslut code here

		return stat;
	case DEV_SPI_FLASH :

		return SPI_FLASH_disk_status();
	}
	return STA_NOINIT;

}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

void SPI_FLASH_disk_read(BYTE *buff, LBA_t sector, UINT count){
	spi_flash_read(((uint32_t)sector * W25X_SECTOR_SIZE), (char *)buff, W25X_SECTOR_SIZE * count);
        return;

}


DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res = STA_NOINIT;
	//int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		// result = RAM_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_MMC :
		// translate the arguments here

		// result = MMC_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_USB :
		// translate the arguments here

		// result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
	case DEV_SPI_FLASH :
		SPI_FLASH_disk_read(buff, sector, count);
		
		return RES_OK;
	}

	return RES_PARERR;
}


/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

void SPI_FLASH_disk_write(const BYTE *buff, LBA_t sector, UINT count){
	spi_flash_write((uint32_t)sector * W25X_SECTOR_SIZE, (char *)buff, count * W25X_SECTOR_SIZE, true);
        return;
}



DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res = STA_NOINIT;
	// int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		// result = RAM_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_MMC :
		// translate the arguments here

		// result = MMC_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_USB :
		// translate the arguments here

		// result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
	case DEV_SPI_FLASH :
		SPI_FLASH_disk_write(buff, sector, count);

		return RES_OK;
	}

	return RES_PARERR;
}


#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = STA_NOINIT;
	// int result;

	switch (pdrv) {
	case DEV_RAM :

		// Process of the command for the RAM drive

		return res;

	case DEV_MMC :

		// Process of the command for the MMC/SD card

		return res;

	case DEV_USB :

		// Process of the command the USB drive

		return res;
	case DEV_SPI_FLASH :
		switch(cmd){
			case GET_SECTOR_COUNT :
				*(DWORD *)buff = (W25X_CAPACITY / W25X_SECTOR_SIZE);
				res = RES_OK;
				break;
			case GET_SECTOR_SIZE :
				*(WORD *)buff = W25X_SECTOR_SIZE;
				res = RES_OK;
				break;
			case GET_BLOCK_SIZE :
				*(DWORD *)buff = 1U;
				res = RES_OK;
				break;
			case CTRL_SYNC :
				res = RES_OK;
			default:
		}
		break;
	default:
	}

	return res;
}

DWORD get_fattime(void) 
{
	uint32_t rtc_cnt = rtc_read_cnt();
        rtc_cnt += GMT_VAL * SEC_IN_HOUR;
        struct date_t d = get_date(rtc_cnt);
        uint32_t year = (uint32_t)d.year;
        uint32_t mon = (uint32_t)d.mon;
        uint32_t day = (uint32_t)d.day;

	struct time_t t = get_time(rtc_cnt);
        uint32_t hour = (uint32_t)t.hour;
        uint32_t min = (uint32_t)t.min;
        uint32_t sec = (uint32_t)t.sec;

	return	(DWORD)((year - 1980) << 25) |
		(DWORD)((mon + 1) << 21) |
		(DWORD)((day + 1) << 16) |
		(DWORD)((hour) << 11) |
		(DWORD)((min) << 5) |
		(DWORD)((sec) << 1) ;




}




