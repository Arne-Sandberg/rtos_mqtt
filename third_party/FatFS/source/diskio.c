/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* by grqd_xp                                                            */
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <string.h>
#include "diskio.h"
#include "sdcard.h"

/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */
/* Note that Tiny-FatFs supports only single drive and always            */
/* accesses drive number 0.                                              */

#define SECTOR_SIZE 512U

//u32 buff2[512/4];
/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
    u8 state;

    if(drv)
    {
        return STA_NOINIT;  //��֧�ִ���0�Ĳ���
    }
    state = SD_Init();
    if(state == STA_NODISK)
    {
        return STA_NODISK;
    }
    else if(state != 0)
    {
        return STA_NOINIT;  //�������󣺳�ʼ��ʧ��
    }
    else
    {
        return 0;           //��ʼ���ɹ�
    }
//	return 0;
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{	
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	u8 res=0;
    if (drv || !count)
    {    
        return RES_PARERR;  //��֧�ֵ����̲�����count���ܵ���0�����򷵻ز�������
    }
	/*
    if(!SD_DET())
    {
        return RES_NOTRDY;  //û�м�⵽SD������NOT READY����
    }*/	
    if(count==1)            //1��sector�Ķ�����      
    {                                                
        res = SD_ReadSingleBlock(sector, buff);      
    }                                                
    else                    //���sector�Ķ�����     
    {                                                
        res = SD_ReadMultiBlock(sector, buff, count);
    }                                                
	/*
    do                           
    {                                          
        if(SD_ReadSingleBlock(sector, buff)!=0)
        {                                      
            res = 1;                           
            break;                             
        }                                      
        buff+=512;                             
    }while(--count);                                         
    */
    //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res == 0x00)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }
/*
  //memset(buff2, 0, sizeof(buff2));
	if(count==1)
    {
          SD_ReadBlock(sector << 9 ,(u32 *)(&buff[0]),SECTOR_SIZE);
          //memcpy(buff,buff2,SECTOR_SIZE);
	}
	else
    {
          SD_ReadMultiBlocks(sector << 9 ,(u32 *)(&buff[0]),SECTOR_SIZE,count);
          //memcpy(buff,buff2,SECTOR_SIZE * count);
	}
	return RES_OK;
*/
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	u8 res;

    if (drv || !count)
    {    
        return RES_PARERR;  //��֧�ֵ����̲�����count���ܵ���0�����򷵻ز�������
    }
	/*
    if(!SD_DET())
    {
        return RES_NOTRDY;  //û�м�⵽SD������NOT READY����
    }*/
    // ��д����
    if(count == 1)
    {
        res = SD_WriteSingleBlock(sector, buff);
    }
    else
    {
        res = SD_WriteMultiBlock(sector, buff, count);
    }
    // ����ֵת��
    if(res == 0)
        return RES_OK;
    else
        return RES_ERROR;
/*
  //memset(buff2, 0, sizeof(buff2));
	if(count==1)
        {
          //memcpy(buff2,buff,SECTOR_SIZE);
          SD_WriteBlock(sector << 9 ,(u32 *)(&buff[0]),SECTOR_SIZE);
	}
	else
        {
          //memcpy(buff2,buff,SECTOR_SIZE * count);
          SD_WriteMultiBlocks(sector << 9 ,(u32 *)(&buff[0]),SECTOR_SIZE,count);
	}       
  return RES_OK;
*/
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    DRESULT res;


    if (drv)
    {    
        return RES_PARERR;  //��֧�ֵ����̲��������򷵻ز�������
    }
    
    //FATFSĿǰ�汾���账��CTRL_SYNC��GET_SECTOR_COUNT��GET_BLOCK_SIZ��������
    switch(ctrl)
    {
    case CTRL_SYNC:
        Clr_SD_CS;
        if(SD_WaitReady()==0)
        {
            res = RES_OK;
        }
        else
        {
            res = RES_ERROR;
        }
        Set_SD_CS;
        break;
        
    case GET_BLOCK_SIZE:
        *(WORD*)buff = 512;
        res = RES_OK;
        break;

    case GET_SECTOR_COUNT:
        *(DWORD*)buff = SD_GetCapacity();
        res = RES_OK;
        break;
    default:
        res = RES_PARERR;
        break;
    }
    return res;
//	return RES_OK;
}

DWORD get_fattime(void){
	return 0;
}

 /*
                                          
DWORD get_fattime (void)
{
    struct tm t;
    DWORD date;
    t = Time_GetCalendarTime();
    t.tm_year -= 1980;		//��ݸ�Ϊ1980����
    t.tm_mon++;         	//0-11�¸�Ϊ1-12��
    t.tm_sec /= 2;      	//��������Ϊ0-29
    
    date = 0;
    date = (t.tm_year << 25)|(t.tm_mon<<21)|(t.tm_mday<<16)|\
            (t.tm_hour<<11)|(t.tm_min<<5)|(t.tm_sec);

    return date;
}
*/
