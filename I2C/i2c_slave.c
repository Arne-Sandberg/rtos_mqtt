/*
i2c_slave.c
QQ:		957165610
Ver 1.0
2014-05-11				by			xulifeng
Ver 1.1
2014-05-11				by			xulifeng
Ver 1.2
2014-05-13				by			xulifeng
*/
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include <stdio.h>
#include <string.h>


#include "i2c_sim.h"
#include "i2c_slave_port.h"

#include "i2c_slave.h"

typedef enum
{
		I2C_SLAVE_IDLE,
		I2C_SLAVE_ADD,//write iic add
		I2C_SLAVE_REG,//write the register add
		I2C_SLAVE_WRITE,//master write and slave read
		I2C_SLAVE_READ,//master read and slave write
		I2C_SLAVE_BUSY
}e_I2C_SLAVE_MODE;

e_I2C_SLAVE_MODE		i2c_slave_mode = I2C_SLAVE_IDLE;
unsigned char I2C_DATA_TEMP = 0;
unsigned char i2c_slave_reg_p = 0;//the register add
unsigned char i2c_slave_data_p = 0;//

// define I2C buf ring

char   g_I2cBufreadIndex = 0;
char   g_I2cBufwriteIndex = 0;
I2cBuff g_I2cBufRing[MAX_I2C_BUF] ;


I2cBuff g_I2cBuf ;







extern void mqtt_notifyI2cData(void);




void I2C_SCL_H(void)
{
	set_gpio_direction(SCL, OUTP);   
	GPIO_SetBits(GPIOB,SCL_PORT);
}

void I2C_SCL_L( void )
{
	set_gpio_direction(SCL, OUTP);   
	GPIO_ResetBits(GPIOB,SCL_PORT);
}

void I2C_SDA_H( void )
{
	set_gpio_direction(SDA, OUTP);   
	GPIO_SetBits(GPIOB,SDA_PORT);

	udelay(20);
	set_gpio_direction(SDA, INP);  

}

void I2C_SDA_L( void )
{
	set_gpio_direction(SDA, OUTP);   
	GPIO_ResetBits(GPIOB,SDA_PORT);

	udelay(20);
	set_gpio_direction(SDA, INP);  
}

#if 0
uint8_t I2C_SCL_IN(void)
{
	uint8_t r;
	set_gpio_direction(SCL, INP);   
	r = GPIO_ReadInputDataBit(GPIOB,SCL_PORT);
	return r;	
}


uint8_t I2C_SDA_IN(void)
{
	uint8_t r;
	set_gpio_direction(SDA, INP);  	
	r = GPIO_ReadInputDataBit(GPIOB,SDA_PORT);
	return r;
}
#endif

void i2c_slave_p( void )
{
	//I2C_SDA_INT_DIS();
	
//	I2C_SCL_L();//slow the i2c speed
	
	if( i2c_slave_data_p > 8 )
	{
		i2c_slave_data_p = 0;
		if( i2c_slave_mode == I2C_SLAVE_READ )
		{
			I2C_SLAVE_READ_BYTE(i2c_slave_reg_p++, &I2C_DATA_TEMP);
			
			if( I2C_DATA_TEMP & 0x80 )//output a bit
				I2C_SDA_H();
			else
				I2C_SDA_L();
			I2C_DATA_TEMP <<= 1;
		}
		else
			I2C_SDA_H();//end ack;
		
		I2C_SCL_H();
		return;
	}

	switch( i2c_slave_mode )
	{
	
		case I2C_SLAVE_ADD:
			if( i2c_slave_data_p == 8 )
			{
		//		printf("ad:%d ",I2C_DATA_TEMP);
					i2c_slave_data_p = 0;

					if( I2C_DATA_TEMP >> 1 == I2C_ADD )
					{
						I2C_SDA_INT_DIS();
						I2C_SDA_L();//ack
						if( I2C_DATA_TEMP & 0x01 )//read
						{
							i2c_slave_mode = I2C_SLAVE_READ;
						}
						else
						{
							i2c_slave_mode = I2C_SLAVE_REG;//write regster add
						}
					}
					else
						i2c_slave_mode = I2C_SLAVE_BUSY;//nack

					I2C_DATA_TEMP = 0;	
					
			}
			break;
			
		case I2C_SLAVE_REG:
			if( i2c_slave_data_p == 8 )
			{
			//	printf("rg:%d ",I2C_DATA_TEMP);		
				g_I2cBuf.reg = I2C_DATA_TEMP;
				
				I2C_SDA_INT_DIS();
				I2C_SDA_L();//ack
				i2c_slave_reg_p = I2C_DATA_TEMP;
				i2c_slave_mode = I2C_SLAVE_WRITE;
				i2c_slave_data_p = 0;
				I2C_DATA_TEMP = 0;	
					
			}
			break;
		
		case I2C_SLAVE_WRITE:
			if( i2c_slave_data_p == 8 )
			{//
	//			printf("dt:%d ",I2C_DATA_TEMP);				
				i2c_slave_data_p = 0;
				I2C_SDA_INT_DIS();
				I2C_SDA_L();

				//save data to buffer
				g_I2cBuf.data[g_I2cBuf.len]	= I2C_DATA_TEMP;
				g_I2cBuf.len++;
				
				
/*
					if(1 == i2c_slave_write_byte( i2c_slave_reg_p++, I2C_DATA_TEMP ) )
					{
							I2C_SDA_INT_DIS();
							I2C_SDA_L();
						  
					}
					else
					{
							i2c_slave_mode = I2C_SLAVE_BUSY;
					}
*/					
			}
			break;
				
		case I2C_SLAVE_READ:
			if( i2c_slave_data_p < 8 )
			{
					if( I2C_DATA_TEMP & 0x80 )//output a bit
						I2C_SDA_H();
					else
						I2C_SDA_L();
					I2C_DATA_TEMP <<= 1;
			}
			if( i2c_slave_data_p == 8 )
			{
					I2C_SDA_H();
					
			}
			default:break;
	}
//	I2C_SCL_H();
}


void i2c_slave_scl_h( void )
{
	I2C_SDA_INT_EN();
	switch( i2c_slave_mode )
	{
		case I2C_SLAVE_ADD:
		case I2C_SLAVE_REG:
		case I2C_SLAVE_WRITE:
			if( (i2c_slave_data_p == 0) && (i2c_slave_mode == I2C_SLAVE_REG))
				I2C_DATA_TEMP = 0;
			
			if(i2c_slave_data_p <8)
			{
				I2C_DATA_TEMP <<= 1;
				if( I2C_SDA_IN() )
					I2C_DATA_TEMP ++;
				i2c_slave_data_p++;

			}
			else
			{
			
				i2c_slave_p();
			}

			break;
		
		case I2C_SLAVE_READ:
			i2c_slave_data_p++;
			if( i2c_slave_data_p > 8 )
			{
				if( I2C_SDA_IN() )
				{
					i2c_slave_mode = I2C_SLAVE_BUSY;
				}
			}
		default:break;
	}


	
}


void i2c_slave_scl_l( void )
{
	I2C_SDA_INT_DIS();
	return;
}

//STOP
void i2c_slave_sda_h( void )
{
		if( I2C_SCL_IN() )
		{
			I2C_SCL_INT_DIS();
			i2c_slave_mode = I2C_SLAVE_IDLE;

			g_I2cBuf.stop=1;

			memcpy(&g_I2cBufRing[g_I2cBufwriteIndex],&g_I2cBuf,sizeof(g_I2cBuf));
			g_I2cBufwriteIndex++;
			if(g_I2cBufwriteIndex >=MAX_I2C_BUF) 
				g_I2cBufwriteIndex = 0;
				

		}
}

//START
void i2c_slave_sda_l( void )
{
	if( I2C_SCL_IN() )
	{
		I2C_SCL_INT_EN();
		i2c_slave_mode = I2C_SLAVE_ADD;
		i2c_slave_data_p = 0;
		g_I2cBuf.stop=0;
		g_I2cBuf.len=0;
	}
}

void i2c_init_buf(void)
{
	g_I2cBuf.len = 0;
	g_I2cBuf.stop=0;
}

I2cBuff * i2c_get_buf(void)
{

	//printf("read:%d,write:%d",g_I2cBufreadIndex,g_I2cBufwriteIndex);
	if(g_I2cBufreadIndex == g_I2cBufwriteIndex)
	{
		//read finish

		return 0;
	}

	return &(g_I2cBufRing[g_I2cBufreadIndex]);
}

I2cBuff * i2c_getNext_buf()
{
	I2cBuff * pbuf;
	g_I2cBufreadIndex ++;
	if(g_I2cBufreadIndex >=MAX_I2C_BUF )
		g_I2cBufreadIndex = 0;

//	printf("read:%d,write:%d",g_I2cBufreadIndex,g_I2cBufwriteIndex);

	if(g_I2cBufreadIndex == g_I2cBufwriteIndex)
	{
		//read finish
		return 0;
	}
	
	pbuf = &(g_I2cBufRing[g_I2cBufreadIndex]);
	//memcpy(pbuf, &g_I2cBuf, sizeof(I2cBuff));
	return pbuf;
}



