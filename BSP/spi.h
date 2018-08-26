
#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x_spi.h"

// SPI�����ٶ����� 
// SPI�����ٶ����� 
#define SPI_SPEED_2   0
#define SPI_SPEED_4   1
#define SPI_SPEED_8   2
#define SPI_SPEED_16  3
#define SPI_SPEED_64  4
#define SPI_SPEED_256 5
						  	    													  
void SPI1_Configuration(void);			 //��ʼ��SPI��
void SPI1_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   
u8 SPI1_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�
		 
#endif
