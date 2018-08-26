
#include "stm32f10x.h"
#include "spi.h"

//Mini STM32开发板
//SPI 驱动 V1.2
//正点原子@ALIENTEK
//2010/6/13	

//以下是SPI模块的初始化代码，配置成主机模式，访问SD Card/W25X16/24L01/JF24C							  
//SPI口初始化
//这里针是对SPI1的初始化
void SPI1_Configuration(void)
{	 
	SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
	//启动SPI1时钟
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|
							RCC_APB2Periph_GPIOB , ENABLE);
	// RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI1, ENABLE ) ;

    //////下面是SPI相关GPIO初始化//////
    //Configure SPI1 pins: SCK, MISO and MOSI 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //复用推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStructure);  
	
	//Configure PB14 pin: SD_CS pin 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStructure); 
	//Configure PB14 pin: SD_CS pin 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//推挽输出
	GPIO_Init(GPIOB,&GPIO_InitStructure); 

	//Configure PB14 pin: SD_PWR pin 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//推挽输出
	GPIO_Init(GPIOC,&GPIO_InitStructure); 

    //////SPI模块配置//////
    //一开始SD初始化阶段，SPI时钟频率必须<400K
    //SD_CS_DISABLE();
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);
	  /* SPI1 configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

	  /* Enable SPI1  */
	SPI_Cmd(SPI1, ENABLE);
	return;	 
}  
	 
//SPI 速度设置函数
//SpeedSet:
//SPI_SPEED_2   2分频   (SPI 36M@sys 72M)
//SPI_SPEED_8   8分频   (SPI 9M@sys 72M)
//SPI_SPEED_16  16分频  (SPI 4.5M@sys 72M)
//SPI_SPEED_256 256分频 (SPI 281.25K@sys 72M)
void SPI1_SetSpeed(u8 SpeedSet)
{
    SPI_InitTypeDef SPI_InitStructure;

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    //如果速度设置输入0，则低速模式，非0则高速模式
	if(SpeedSet==SPI_SPEED_2)
    {
        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    }
    else	if(SpeedSet==SPI_SPEED_8)
    {
        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    }
	 else	 if(SpeedSet==SPI_SPEED_16)
    {
        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    }
	else	 if(SpeedSet==SPI_SPEED_64)
    {
        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    }
	 else
    {
        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    }
	//moon.mp3: 4707774 Byte size 目标文件 设为buffer[512]     
	//speed:实验测试数据，最大速度 392314 Byte/S，
	//Prescaler_128, 59592 Byte/S
	//Prescaler_64, 104617 Byte/S
	//Prescaler_32, 168134 Byte/S    162337 Byte/S
	//Prescaler_16, 261543 Byte/S    247777 Byte/S
	//Prescaler_8,  313851 Byte/S    336269 Byte/S
	//Prescaler_4,  392314 Byte/S    392314 Byte/S
	//Prescaler_2,  392314 Byte/S

	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);
    return;
}

//SPI1 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI1_ReadWriteByte(u8 TxData)
{		
    u8 RxData = 0;
    
    //等待发送缓冲区空
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    //发一个字节
    SPI_I2S_SendData(SPI1, TxData);

    //等待数据接收
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    //取数据
    RxData = SPI_I2S_ReceiveData(SPI1);

    return (u8)RxData;	
				    
}
