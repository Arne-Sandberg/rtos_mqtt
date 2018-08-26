#ifndef __I2C_SLAVE_H__
#define __I2C_SLAVE_H__

#define I2C_ADD			0x5a

#define SDA_PORT GPIO_Pin_9                         //定义SDA所对应的GPIO接口编号 
#define SCL_PORT GPIO_Pin_8                         //定义SCL所对应的GPIO接口编号  

#if 0
#define I2C_SCL_H()				GPIO_SetBits(GPIOB,SCL_PORT)
#define I2C_SCL_L()				GPIO_ResetBits(GPIOB,SCL_PORT)
#define I2C_SDA_H()				GPIO_SetBits(GPIOB,SDA_PORT)
#define I2C_SDA_L()				GPIO_ResetBits(GPIOB,SDA_PORT)
#endif

#define I2C_SCL_IN()			GPIO_ReadInputDataBit(GPIOB,SCL_PORT)
#define I2C_SDA_IN()			GPIO_ReadInputDataBit(GPIOB,SDA_PORT)


#define I2C_SDA_INT_EN()								i2c_slave_sda_int_en()
#define I2C_SDA_INT_DIS()								i2c_slave_sda_int_dis()
#define I2C_SCL_INT_EN()								i2c_slave_scl_int_en()
#define I2C_SCL_INT_DIS()								i2c_slave_scl_int_dis()
#define I2C_SLAVE_WRITE_BYTE(reg,x)			i2c_slave_write_byte(reg,x)
#define I2C_SLAVE_READ_BYTE(reg,x)			i2c_slave_read_byte(reg,x)


#define MAX_I2C_BUF 16


typedef struct _I2cBuff
{
	unsigned char reg;
	unsigned char len;
	unsigned char data[30];
	unsigned char stop;
} I2cBuff;

void i2c_slave_p( void );


void i2c_slave_scl_h( void );
void i2c_slave_scl_l( void );

void i2c_slave_sda_h( void );
void i2c_slave_sda_l( void );

void i2c_init_buf(void);

I2cBuff * i2c_get_buf(void);
I2cBuff * i2c_getNext_buf(void);


void slave_i2c_init( void );

#endif
