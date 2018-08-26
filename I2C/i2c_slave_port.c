#include "stm32f10x.h"
#include "i2c_slave.h"
#include "i2c_slave_port.h"


#define OUTPUT GPIO_Mode_Out_PP                  //表示GPIO接口方向为输出  
#define INPUT GPIO_Mode_IPU                //表示GPIO接口方向为输入 


void Exit_Config( void )
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

#if 0 
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable GPIOA clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Connect EXTI Line2 to PA2 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource2);
	/* Connect EXTI Line3 to PA3 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource3);

	/* Configure EXTI Line2 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	/* Configure EXTI Line3 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI Line2 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

#endif


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin = SDA_PORT | SCL_PORT ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = INPUT;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Connect EXTI Line8 to PB8 pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource9);  //?
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);  //?

	/* Configure EXTI Line8 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	/* Configure EXTI Line9 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line9;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);



	/* Enable and set EXTI Line2 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


}

void i2c_slave_sda_int_en( void )
{
		EXTI_InitTypeDef EXTI_InitStructure;
	
		EXTI_ClearITPendingBit(EXTI_Line9);
		EXTI_InitStructure.EXTI_Line = EXTI_Line9;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
}
void i2c_slave_scl_int_en( void )
{
		EXTI_InitTypeDef EXTI_InitStructure;
	
		EXTI_ClearITPendingBit(EXTI_Line8);
		EXTI_InitStructure.EXTI_Line = EXTI_Line8;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
}


void i2c_slave_port_init( void )
{
		GPIO_InitTypeDef  GPIO_InitStructure;
		
		/*************************GPIO*******************/
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 
		
		GPIO_InitStructure.GPIO_Pin = SDA_PORT | SCL_PORT ;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = INPUT;
		GPIO_Init(GPIOB, &GPIO_InitStructure);	

	//	I2C_SCL_H();
	//	I2C_SDA_H();

		Exit_Config();
		i2c_slave_sda_int_en();
}

void slave_i2c_init( void )
{
		i2c_slave_port_init();
		i2c_init_buf();
}


void i2c_slave_scl_int_dis( void )
{
		EXTI_InitTypeDef EXTI_InitStructure;
	
		EXTI_InitStructure.EXTI_Line = EXTI_Line8;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
		EXTI_InitStructure.EXTI_LineCmd = DISABLE;
		EXTI_Init(&EXTI_InitStructure);
}

void i2c_slave_sda_int_dis( void )
{
		EXTI_InitTypeDef EXTI_InitStructure;
	
		EXTI_InitStructure.EXTI_Line = EXTI_Line9;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
		EXTI_InitStructure.EXTI_LineCmd = DISABLE;
		EXTI_Init(&EXTI_InitStructure);
}



static unsigned char reg_data[32] = {0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x88};

char i2c_slave_write_byte( unsigned char reg, unsigned char x )
{
		reg_data[reg] = x;
		return 1;
}

char i2c_slave_read_byte( unsigned char reg, unsigned char *x_p )
{
		*x_p = reg_data[reg];
		return 1;
}


/* interrupt service routine */
void EXTI9_5_IRQHandler ( void )
{
	if( EXTI_GetFlagStatus(EXTI_Line8) )
	{
		EXTI_ClearITPendingBit(EXTI_Line8);
		if( I2C_SCL_IN() )
		{
			i2c_slave_scl_h();
		}
		else
			i2c_slave_scl_l();
	}
	
	else if( EXTI_GetFlagStatus(EXTI_Line9) )
	{
		EXTI_ClearITPendingBit(EXTI_Line9);
		if( I2C_SDA_IN() )
			i2c_slave_sda_h();
		else
			i2c_slave_sda_l();
	}
}

