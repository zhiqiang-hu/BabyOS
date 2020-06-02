/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "b_os.h"
/* Definitions of physical drive number for each drive */
#define DEV_SPIFLASH		0	/* Map SPIFLASH to physical drive 0 */
#define DEV_SDCARD          1	/* Map MMC/SD card to physical drive 1 */



/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = RES_OK;
	switch (pdrv) {
	case DEV_SPIFLASH :
		return stat;

	case DEV_SDCARD :
		return stat;
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
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
    UINT i = 0;
	DRESULT res = RES_OK;
    int fd = -1;
	switch (pdrv) {
	case DEV_SPIFLASH :
		// translate the arguments here

		fd = bOpen(SPIFLASH, BCORE_FLAG_RW);
        if(fd >= 0)
        {
            bLseek(fd, sector * 4096);
            for(i = 0;i < count;i++)
            {
                bRead(fd, buff + i * 4096, 4096);
            }
            bClose(fd);
        }
        else
        {
            res = RES_ERROR;
        }

		// translate the reslut code here

		return res;

	case DEV_SDCARD :
		// translate the arguments here

		fd = bOpen(SD, BCORE_FLAG_RW);
        if(fd >= 0)
        {
            bLseek(fd, sector);
            bRead(fd, buff, count);
            bClose(fd);
        }
        else
        {
            res = RES_ERROR;
        }

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	UINT i = 0;
    bCMD_Erase_t cmd_erase;
	DRESULT res = RES_OK;
    int fd = -1;
	switch (pdrv) {
	case DEV_SPIFLASH :
		// translate the arguments here
        
		fd = bOpen(SPIFLASH, BCORE_FLAG_RW);
        if(fd >= 0)
        {
            cmd_erase.addr = sector * 4096;
            cmd_erase.num = count;
            bCtl(fd, bCMD_ERASE_SECTOR, &cmd_erase);
            bLseek(fd, sector * 4096);
            for(i = 0;i < count;i++)
            {
                bWrite(fd, (uint8_t *)buff + i * 4096, 4096);
            }
            bClose(fd);
        }
        else
        {
            res = RES_ERROR;
        }

		// translate the reslut code here

		return res;

	case DEV_SDCARD :
		// translate the arguments here

		fd = bOpen(SD, BCORE_FLAG_RW);
        if(fd >= 0)
        {
            bLseek(fd, sector);
            bWrite(fd, (uint8_t *)buff, count);
            bClose(fd);
        }
        else
        {
            res = RES_ERROR;
        }

		// translate the reslut code here

		return res;
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
	DRESULT res = RES_OK;
	switch (pdrv) {
	case DEV_SPIFLASH :
        switch(cmd)
        {
            case GET_SECTOR_COUNT:
                ((LBA_t *)buff)[0] = 2000;
                break;
            case GET_SECTOR_SIZE:
                ((WORD *)buff)[0] = 4096;
                break;
            case GET_BLOCK_SIZE:
                ((WORD *)buff)[0] = 1;
                break;
        }
		// Process of the command for the RAM drive

		return res;

	case DEV_SDCARD :
        switch(cmd)
        {
            case GET_SECTOR_COUNT:
                ((LBA_t *)buff)[0] = 65536;
                break;
            case GET_SECTOR_SIZE:
                ((WORD *)buff)[0] = 512;
                break;
            case GET_BLOCK_SIZE:
                ((WORD *)buff)[0] = 1;
                break;
        }
		// Process of the command for the MMC/SD card

		return res;
	}

	return RES_PARERR;
}

