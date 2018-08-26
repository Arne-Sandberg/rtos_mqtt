#ifndef __I2C_SIM_H__
#define __I2C_SIM_H__



#define IPORT GPIOB
#define RCC_PORT RCC_APB2Periph_GPIOB

#define SRC_PORT GPIO_PortSourceGPIOB


#define SDA GPIO_Pin_9                         //定义SDA所对应的GPIO接口编号 
#define SCL GPIO_Pin_8                         //定义SCL所对应的GPIO接口编号  
#define OUTP GPIO_Mode_Out_PP                  //表示GPIO接口方向为输出  
#define INP GPIO_Mode_IPU                //表示GPIO接口方向为输入 



#define SCL_HIGH         IPORT->BSRR = SCL 
#define SCL_LOW          IPORT->BRR  = SCL 

#define SDA_HIGH         IPORT->BSRR = SDA 
#define SDA_LOW          IPORT->BRR  = SDA 

#define SCL_read         IPORT->IDR  & SCL 
#define SDA_read         IPORT->IDR  & SDA 


void simI2c_master_init(void);

void set_gpio_direction(uint16_t pin, unsigned char mode);

void udelay(u32 i);


#endif

