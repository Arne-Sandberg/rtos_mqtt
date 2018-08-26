
#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x_spi.h"

// SPI总线速度设置 
// SPI总线速度设置 
#define SPI_SPEED_2   0
#define SPI_SPEED_4   1
#define SPI_SPEED_8   2
#define SPI_SPEED_16  3
#define SPI_SPEED_64  4
#define SPI_SPEED_256 5
						  	    													  
void SPI1_Configuration(void);			 //初始化SPI口
void SPI1_SetSpeed(u8 SpeedSet); //设置SPI速度   
u8 SPI1_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
		 
#endif
