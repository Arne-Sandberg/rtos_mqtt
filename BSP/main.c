\
#include "stm32f10x.h"
#include "stm32f107.h"

#include "FreeRTOS.h"
#include "task.h"
#include "common.h"
#include "timers.h"

#include "queue.h"
#include "semphr.h"
#include "utils.h"
#include "configApp.h"
#define _DEBUG
#include "dprintf.h"
#include "i2c_slave.h"
#include "loggerManage.h"
#include "watchdog.h"

extern void mqttTaskCreate(void);
extern void configTaskCreate(void);
extern void collectTaskCreate(void);



#if 0
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Configure IO connected to LD1, LD2, LD3 and LD4 leds *********************/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

//??????
void NVIC_Configuration(void)
{ 
  	/* Configure the NVIC Preemption Priority Bits */  
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	#ifdef  VECT_TAB_RAM  
	  /* Set the Vector Table base location at 0x20000000 */ 
	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else  /* VECT_TAB_FLASH  */
	  /* Set the Vector Table base location at 0x08000000 */ 
	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	#endif
}


void RCC_Configuration(void)
{
	SystemInit();	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA 
                           |RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC
                           |RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE
						   |RCC_APB2Periph_ADC1  | RCC_APB2Periph_AFIO 
                           |RCC_APB2Periph_SPI1, ENABLE );
  // RCC_APB2PeriphClockCmd(RCC_APB2Periph_ALL ,ENABLE );
     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 
                           |RCC_APB1Periph_USART3|RCC_APB1Periph_TIM2	                           
                           , ENABLE );
	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

#endif 

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	/* This function will get called if a task overflows its stack.   If the
	parameters are corrupt then inspect pxCurrentTCB to find which was the
	offending task. */

	( void ) pxTask;
	printf("task:%s stack overflow!\n", pcTaskName);
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
}

#if 0
static void prvSetupHardware( void )
{
	/* Start with the clocks in their expected state. */
	RCC_DeInit();

	/* Enable HSE (high speed external clock). */
	RCC_HSEConfig( RCC_HSE_ON );

	/* Wait till HSE is ready. */
	while( RCC_GetFlagStatus( RCC_FLAG_HSERDY ) == RESET )
	{
	}

	/* 2 wait states required on the flash. */
	*( ( unsigned long * ) 0x40022000 ) = 0x02;

	/* HCLK = SYSCLK */
	RCC_HCLKConfig( RCC_SYSCLK_Div1 );

	/* PCLK2 = HCLK */
	RCC_PCLK2Config( RCC_HCLK_Div1 );

	/* PCLK1 = HCLK/2 */
	RCC_PCLK1Config( RCC_HCLK_Div2 );

	/* Enable PLL. */
	RCC_PLLCmd( ENABLE );

	/* Wait till PLL is ready. */
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	{
	}

	/* Select PLL as system clock source. */
	RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );

	/* Wait till PLL is used as system clock source. */
	while( RCC_GetSYSCLKSource() != 0x08 )
	{
	}

	/* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE and AFIO clocks */
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC
							| RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE );

	/* Set the Vector Table base address at 0x08000000 */
	NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x0 );

	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

	/* Configure HCLK clock as SysTick clock source. */
	SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );

	GPIO_Configuration();


}
#endif


#define ledSTACK_SIZE		configMINIMAL_STACK_SIZE
#define ledFLASH_RATE_BASE	( ( TickType_t ) 333 )
#if 0
static TaskHandle_t xHandleTaskLED=NULL;


static void vTaskLED(void* pvParameters)
{
	TickType_t xFlashRate, xLastFlashTime;
	xFlashRate = ledFLASH_RATE_BASE + ( ledFLASH_RATE_BASE * ( TickType_t ) 2 );
	xFlashRate /= portTICK_PERIOD_MS;
	xFlashRate /= ( TickType_t ) 2;
	xLastFlashTime = xTaskGetTickCount();

 	while(1)
  	{
		/* Turn on LD1 */
    	GPIO_SetBits(GPIOD, GPIO_Pin_2);
    	/* Insert delay */
    	//vTaskDelay(300);
		vTaskDelayUntil( &xLastFlashTime, xFlashRate );

     	/* Turn off LD1 */
    	GPIO_ResetBits(GPIOD, GPIO_Pin_2);
     	/* Insert delay */
    	//vTaskDelay(300);
		vTaskDelayUntil( &xLastFlashTime, xFlashRate );
  	}
}
#endif

#include "serial.h"
#include "integer.h"
#include "diskio.h"
#include "ff.h"
#include "sdcard.h" 
#include "common.h"
#include <stdio.h>


#define SYSTEM_INIT_TASK_PRIO    ( tskIDLE_PRIORITY )

/* Handle to the com port used by both tasks. */
//static xComPortHandle xPort = NULL;

/* The Rx task will block on the Rx queue for a long period. */
#define comRX_BLOCK_TIME			( ( TickType_t ) 0xffff )

#if 0
static void sd_card_task(void* pvParameters)
{ 
	signed char  key = 0;

  GPIO_ResetBits(GPIOF,  GPIO_Pin_6);				                    //LED1 on
  
  while(1)
  {
		printf("\r\n============ ÇëÑ¡Ôñ...===============\r\n\n");
		printf("¸ñ Ê½ »¯------------------------------- 1\r\n\n");
		printf("´´½¨ÎÄ¼ş------------------------------- 2\r\n\n");
		printf("É¾³ıÎÄ¼ş ------------------------------ 3\r\n\n");
		printf("ÁĞ±íÎÄ¼ş------------------------------- 4\r\n\n");
		printf("ÖØÆôÏµÍ³ ------------------------------ 5\r\n\n");
		printf("´ÅÅÌĞÅÏ¢------------------------------- 6\r\n\n");
		printf("´´½¨Ä¿Â¼------------------------------- 7\r\n\n");
		printf("±à¼­ÎÄ¼ş------------------------------- 8\r\n\n");
		printf("¶ÁÈ¡ÎÄ¼ş------------------------------- 9\r\n\n");
		printf("========================================\r\n\n");

		

		xSerialGetChar( xPort, &key, comRX_BLOCK_TIME );
		
		if (key == 0x31)	    //Format FS
		{
			printf("\r\n----1----");
			format_disk();
		}
		else if (key == 0x32)	//Creat File
		{
			printf("\r\n----2----");
			creat_file();		
		}
		else if (key == 0x33)	//Delete File
		{
			printf("\r\n----3----");
			delete_file();		
		}
		else if (key == 0x34)	//List File
		{
			printf("\r\n----4----");
			list_file();
		}
		else if (key == 0x35)   //Reset FS
		{
			printf("\r\n----5----");
			Sys_Soft_Reset();
		}
		else if (key == 0x36)	//Disk info
		{
			printf("\r\n----6----");
			get_disk_info();	
		}
		else if (key == 0x37)	//Creat Dir
		{
			printf("\r\n----7----");
			creat_dir();
		}
		else if (key == 0x38)	//Edit File
		{
			printf("\r\n----8----");
			edit_file();
		}
		else if (key == 0x39)	//Read File
		{
			printf("\r\n----9----");
			read_file();	
		}
		else
		{
			printf("\r\n----%c----",key);
			printf("\r\nÖ»½ÓÊÜ1-9ÃüÁî£¬ÇëÖØĞÂÊäÈë");
		}			
	}		
}
#endif

#include "netconf.h"
#include "stm32_eth.h"


#define SYSTEM_INIT_TASK_PRIO    ( tskIDLE_PRIORITY )
#define DHCP_TASK_PRIO   ( tskIDLE_PRIORITY + 2 )   

static TaskHandle_t xHandleTaskSystemInit=NULL;

static void System_Init_task(void* pvParameters)
{

	printf("system hardware OK !\r\n");

	//xTaskCreate(vTaskLED,"vTaskLED",ledSTACK_SIZE,NULL,3,&xHandleTaskLED);
	//printf("2 led demos tasks are created !\r\n");
	
	//xTaskCreate(sd_card_task,"vTaskSDCard",1024,NULL,SYSTEM_INIT_TASK_PRIO,NULL);	
	//printf("The task of SDCard is created!\r\n");

//	printf("eth hardware initialzing...\r\n");
	//FreeRTOS_Hardware_STMS32_ETH_Init();
//	Ethernet_Configuration();

	printf("eth hardware ok\r\nstarting LwIP stack... ");

	/* Initilaize the LwIP stack */
	LwIP_Init();
	printf("ok \r\n");



	/* Initialize tcp echo server */
	//tcpecho_init();
  
	#ifdef USE_DHCP
	/* Start DHCPClient */
	xTaskCreate(LwIP_DHCP_task, "DHCPClient", configMINIMAL_STACK_SIZE * 2, NULL,DHCP_TASK_PRIO, NULL);
	printf("LwIP_DHCP_task is created !\r\n");
	#endif	

	printf("starting I2c bus task... ");
	slave_i2c_init();
	printf("ok \r\n");

	printf("starting MQTT task... ");
	//enable mqtt
	mqttTaskCreate();
	printf("ok \r\n");
	
	printf("starting data collect task... ");
	collectTaskCreate();
	printf("ok \r\n");

	printf("init logger Management... ");
	initLoggerMng();
	printf("ok \r\n");


	printf("starting Config task... ");
	configTaskCreate();
	printf("ok \r\n");	
	printf("[eIoT]");

	vTaskDelete(xHandleTaskSystemInit);
}



TimerHandle_t wdgTimerHandle;

static void wdgTimerCallback(xTimerHandle pxTimer)
{
    IWDG_Feed();

}


int main(void)
{  

	const TickType_t  xTimerPer = 100;	//tick = 1ms,100ms æ¯100ms å–‚ç‹—

	__set_PRIMASK(1);//½ûÖ¹È«¾ÖÖĞ¶Ï
	//prvSetupHardware();	
	System_Setup();
	FreeRTOS_printf_service_init();

	printf("#\r\n");
	printf("###############################################\r\n");
	printf("##    eIOT gateway--welcome to eIoT          ##\r\n");
	printf("##    version : 1.0.1                        ##\r\n");
	printf("###############################################\r\n");

	printf("\r\n\r\n");
	
	  /* æ£€æŸ¥æ˜¯å¦ä¸ºç‹¬ç«‹çœ‹é—¨ç‹—å¤ä½ */
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
	  /* ç‹¬ç«‹çœ‹é—¨ç‹—å¤ä½ */
  	  printf("reset by watchdog!\r\n");
	
	  /* æ¸…é™¤æ ‡å¿— */
	  RCC_ClearFlag();
	}


	/*start report peroid timer*/

	
	wdgTimerHandle = xTimerCreate("wdg",		   /* å®šæ—¶å™¨åå­— */
							   xTimerPer,	 /* å®šæ—¶å™¨å‘¨æœŸ,å•ä½æ—¶é’ŸèŠ‚æ‹ */
							   pdTRUE,			/* å‘¨æœŸæ€§ */
							   ( void * ) 0,
							   wdgTimerCallback); /* å®šæ—¶å™¨å›è°ƒå‡½æ•° */

	if(wdgTimerHandle == NULL)
	{
		/* æ²¡æœ‰åˆ›å»ºæˆåŠŸï¼Œç”¨æˆ·å¯ä»¥åœ¨è¿™é‡ŒåŠ å…¥åˆ›å»ºå¤±è´¥çš„å¤„ç†æœºåˆ¶ */
	   printf("watchdog timer fail!!\r\n");
	}
	else
	{
		 /* å¯åŠ¨å®šæ—¶å™¨ï¼Œç³»ç»Ÿå¯åŠ¨åæ‰å¼€å§‹å·¥ä½œ */
		 if(xTimerStart(wdgTimerHandle, 10) != pdPASS)//ç­‰å¾…å»¶æ—¶100ms
		 	{
			 /* å®šæ—¶å™¨è¿˜æ²¡æœ‰è¿›å…¥æ¿€æ´»çŠ¶æ€ */
		 	}
	 }
	


	//SD_Init(); 
	
	//InitMQTTServerInfo();
	config_Recover();
	
	xTaskCreate(System_Init_task,"vTaskSystemInit",100,NULL,SYSTEM_INIT_TASK_PRIO,&xHandleTaskSystemInit);

	
    // IWDG 4s è¶…æ—¶æº¢å‡º
    IWDG_Config(IWDG_Prescaler_256 ,625); 	//4S WATCHDOG
	
	vTaskStartScheduler();//Æô¶¯ÈÎÎñµ÷¶ÈÆ÷
}

/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/


