/* Library includes. */
#include "stm32f10x.h"
#include "i2c_sim.h"

#include <stdio.h>
#include <string.h>


#define DELAY_TIME_IC 0x00f0
#define DELAY2 0x040



/*
#define SDA_LOW 		GPIO_ResetBits(IPORT , SDA)
#define SCL_LOW 		GPIO_ResetBits(IPORT , SCL)

#define SDA_HIGH 		GPIO_SetBits(IPORT , SDA)
#define SCL_HIGH 		GPIO_SetBits(IPORT , SCL)
*/

void udelay(u32 i)
{
	int j;
	for(;i>0;i--)
		j++;
		//_nop();
}


/*
void I2C_delay(void)
{
  
   u8 i=20; //这里可以优化速度 ，经测试最低到5还能写入
   while(i)
   {
     i--;
   } 
}
*/


void set_gpio_direction(uint16_t pin, unsigned char mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;	

	GPIO_InitStructure.GPIO_Pin = pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = (GPIOMode_TypeDef)mode;
	GPIO_Init(IPORT, &GPIO_InitStructure);	
}

unsigned char get_gpio_value(uint16_t pin){

	return GPIO_ReadInputDataBit(IPORT,pin);
}


void simI2c_master_init(void)
{
	GPIO_DeInit(IPORT);
	RCC_APB2PeriphClockCmd( RCC_PORT , ENABLE); 	
	set_gpio_direction(SDA | SCL,OUTP);

}



/* I2C起始条件 */  
int simI2c_start()  
{

	//初始设置E3/E4全部输出		
	set_gpio_direction(SDA | SCL,OUTP);
	

    SDA_HIGH;             //设置SDA为高电平  
    SCL_HIGH;             //设置SCL为高电平  
	udelay(DELAY_TIME_IC);          //延时  
	if(!SDA_read)return 0; //SDA线为低电平则总线忙,退出	
				  		 //起始条件  
	SDA_LOW;            //SCL为高电平时，SDA由高变低  
	udelay(DELAY_TIME_IC); 
	if(SDA_read) return 0; //SDA线为高电平则总线出错,退出

	return 1;
} 





/* I2C终止条件 */  
void simI2c_stop()  
{  
	set_gpio_direction(SDA, OUTP);  

	//SCL高电平时，SDA由低变高  
	SCL_HIGH;
	SDA_LOW;
	udelay(DELAY_TIME_IC); 
	SDA_HIGH;     
}



  
/*   
I2C读取ACK信号(写数据时使用)  
返回值 ：0表示ACK信号有效；非0表示ACK信号无效  
*/  
unsigned char simI2c_read_ack()  
{  
	unsigned char r =0;  

	set_gpio_direction(SDA, INP);       //设置SDA方向为输入  

	SCL_HIGH;    // SCL变高  
	udelay(DELAY_TIME_IC);  
	r = get_gpio_value(SDA);            //读取ACK信号  
/*	if(r != 0)
	{
		SCL_LOW;							// SCL变低  
		udelay(DELAY_TIME_IC);
	}*/
	SCL_LOW;		
	return r;  
} 


/* I2C发出ACK信号(读数据时使用) */  
void simI2c_send_ack()  
{  
	set_gpio_direction(SDA, OUTP);          //设置SDA方向为输出  
	SCL_LOW;                                // SCL变低  
	SDA_LOW;                                //发出ACK信号  
	udelay(DELAY_TIME_IC);  
	SCL_HIGH;             // SCL变高  
	udelay(DELAY_TIME_IC);  
} 


/* I2C字节写 */  
void simI2c_write_byte(unsigned char b)  
{  
	int i; 
	unsigned char r = 0;  
	set_gpio_direction(SDA, OUTP);          //设置SDA方向为输出  
	
	for (i=7; i>=0; i--) 
	{  
		SCL_LOW;             // SCL变低  
		udelay(DELAY_TIME_IC);  

		//从高位到低位依次准备数据进行发送  
		if(b & (1<<i))
			SDA_HIGH;
		else
			SDA_LOW;
		
		udelay(DELAY2);  // guojun

		SCL_HIGH;

		udelay(DELAY_TIME_IC);  
	}  
	SCL_LOW;
	SDA_HIGH; 
          
	udelay(DELAY_TIME_IC); 	
//	SCL_HIGH;
//	udelay(DELAY_TIME_IC); 
	r = simI2c_read_ack();				   //检查目标设备的ACK信号  
	//0:ACK 1:NCK 
	if(r)
	{
		printf("nak?!?");
	}

/*	
	for(i=0;i<4;i++)			//如果四次读不到，就放弃
	{
		r = simI2c_read_ack();				   //检查目标设备的ACK信号  
		if(r != 0)
		{
			// 0:ACK 1:NCK 
			break;
		}
	}
	*/
} 


/* I2C字节读 */  
unsigned char simI2c_read_byte()  
{  
	int i;  
	unsigned char r = 0;  
	set_gpio_direction(SDA, INP);           //设置SDA方向为输入  
	for (i=7; i>=0; i--) 
	{
		SCL_LOW;         // SCL变低  
		udelay(DELAY_TIME_IC);  
		r = (r <<1) | get_gpio_value(SDA);      //从高位到低位依次准备数据进行读取  
		SCL_HIGH;         // SCL变高  
		udelay(DELAY_TIME_IC);  
	}  
	simI2c_send_ack();                 //向目标设备发送ACK信号  
	return r;  
} 


/*  
I2C读操作  
addr：目标设备地址  
buf：读缓冲区  
len：读入字节的长度  
*/  
void simI2c_read(unsigned char addr, unsigned char* buf, int len)  
{  
	int i;  
	unsigned char t;  
	simI2c_start();                        //起始条件，开始数据通信  
	
	//发送地址和数据读写方向  
	t = (addr << 1) | 1;                    //低位为1，表示读数据  
	simI2c_write_byte(t);  

	//读入数据  
	for (i=0; i<len; i++)  
	buf[i] = simI2c_read_byte();  
	simI2c_stop();                     //终止条件，结束数据通信  
}

  
/*  
I2C写操作  
addr：目标设备地址  
buf：写缓冲区  
len：写入字节的长度  
*/  
void simI2c_write (unsigned char addr, unsigned char* buf, int len)  
{  
	int i;  
	unsigned char t;  
	simI2c_start();                        //起始条件，开始数据通信  

	//发送地址和数据读写方向  
	t = (addr << 1) | 0;                    //低位为0，表示写数据  
	simI2c_write_byte(t);  

	//写入数据  
	for (i=0; i<len; i++)  
		simI2c_write_byte(buf[i]);  
	simI2c_stop();                     //终止条件，结束数据通信  
} 

