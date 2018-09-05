#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
//#include "MQTTClient.h"
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "serial.h"
#include <string.h>



#include "configApp.h"
#include "utils.h"

//#define _DEBUG
#include "dprintf.h"

#include "i2c_sim.h"
#include "i2c_slave.h"
#include "loggerManage.h"

#define SOFTWARE_VERSION 103

static Config_data config;

static unsigned char RS232_buff[RS232_REC_BUFF_SIZE];
char words[32];


/* The Rx task will block on the Rx queue for a long period. */
#define comRX_BLOCK_TIME			( ( TickType_t ) 0xffff )

extern void simI2c_write (unsigned char addr, unsigned char* buf, int len)  ;

/*******************************************************************************
* 函数名：Write_Flash
*描述    ：写STM32指定地址的Flash
*输入    ：buff:写入数据缓冲区，len:写入数据长度
*输出    ：无
*返回值    ：u8：写成功返回1，失败返回0
*说明    ：无
*******************************************************************************/
u8 Write_Flash(u32 *buff, u8 len)
{    
    volatile FLASH_Status FLASHStatus;
    u8 k=0;
    u32 Address;

    Address = CONFIG_FLASH_ADDRESS;
    FLASHStatus = FLASH_COMPLETE;
    FLASH_Unlock();//解锁
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清除所有标志
    FLASHStatus = FLASH_ErasePage(CONFIG_FLASH_ADDRESS);//扇区擦除
    if(FLASHStatus == FLASH_COMPLETE)
    {
        for(k=0;(k<len) && (FLASHStatus == FLASH_COMPLETE);k++)
        {
            FLASHStatus = FLASH_ProgramWord(Address, buff[k]);//写入一个字（32位）的数据入指定地址
            Address = Address + 4;//地址偏移4个字节
        }        
        FLASH_Lock();//重新上锁，防止误写入
    }
    else
    {
        return 0;
    }
    if(FLASHStatus == FLASH_COMPLETE)
    {
        return 1;
    }
    return 0;
}


/*******************************************************************************
* 函数名：Read_Flash
*描述    ：读STM32指定地址的Flash
*输入    ：buff:读出数据缓冲区，len:读出数据长度
*输出    ：无
*返回值    ：无
*说明    ：无
*******************************************************************************/
void Read_Flash(u32 *buff, u8 len)
{
    u8 k;
    u32 Address;
    
    Address = CONFIG_FLASH_ADDRESS;
    for(k=0;k<len;k++)
    {
        buff[k] =  (*(vu32*) Address);//读指定地址的一个字的数据
        Address += 4;//地址偏移4个字节        
    }
} 


void config_mqtt	(char item,char * string)
{
	

	Config_data data;
	
	//read the old config
	Read_Flash((u32 *)&data,sizeof(Config_data)/sizeof(u32));	

	if(item == CONF_MQTT_SERVER)
	{
		if(UTILS_IsIPV4(string) == TRUE)
		{
			UTILS_StrToIP(string, data.mqtt_server_ip);
 		}
		else
		{
			printf("invalid IP address\r\n");
			return;
		}
 	}
	
	if(item == CONF_MQTT_PORT)
	{
		data.mqtt_server_port =(u32) atoi(string);
		
	}
	
	if(1 != Write_Flash((u32 *)&data,sizeof(Config_data)/sizeof(u32)))
	{
		printf("save ip address error\r\n");
	}


	return;
}



void config_mac(char * macStr)
{

	Config_data data;

	
	//read the old config
	Read_Flash((u32 *)&data,sizeof(Config_data)/sizeof(u32));	
	
	UTILS_StrToMac(macStr,data.mac);

	
	if(1 != Write_Flash((u32 *)&data,sizeof(Config_data)/sizeof(u32)))
	{
		printf("save mac error\r\n");
	}	
	return;
}


void config_IPAddress(char * ipaddr)
{

	Config_data data;
	
	//read the old config
	Read_Flash((u32 *)&data,sizeof(Config_data)/sizeof(u32));	
	
	//write ip address to eerom
	if(UTILS_IsIPV4(ipaddr) == TRUE)
	{
		UTILS_StrToIP(ipaddr, data.ip);	
	}
	else
	{
		printf("invalid IP address\r\n");
		return;
	}
	
	if(1 != Write_Flash((u32 *)&data,sizeof(Config_data)/sizeof(u32)))
	{
		printf("save ip address error\r\n");
	}	
	return;
}




void config_Show(char * str)
{
	Config_data data;

	if(0 == strcmp((char *)str,"logger"))
	{
		lgm_ShowAll();
		return;
	}


	if(0 != strcmp((char *)str,"config"))
	{
		printf("\r\n invalid command!\r\n");
		return;
	}
	//read the old config
	Read_Flash((u32 *)&data,sizeof(Config_data)/sizeof(u32));		

	printf("\r\n---------------------------------------");
	printf("\r\nHost ip          : %d.%d.%d.%d",data.ip[0],data.ip[1],data.ip[2],data.ip[3]);	
	printf("\r\nMQTT server ip   : %d.%d.%d.%d",data.mqtt_server_ip[0],data.mqtt_server_ip[1],data.mqtt_server_ip[2],data.mqtt_server_ip[3]);		
	printf("\r\nMQTT server port : %d",data.mqtt_server_port);		
	printf("\r\n---------------------------------------");


	return;
}

void config_command(char * cmdStr,signed char len)
{
	unsigned char action = CONF_INVALID;
	unsigned char i,wordnum=0;
	unsigned char wordlen;

	
	wordlen = 0;	

	for(i=0; i<len; i++)
	{
		if(wordnum>= 2) continue; //only support two words

		if(cmdStr[i] == KEY_SPACE )
		{
			if(wordlen == 0) continue;

			words[wordlen] = 0;
			if(wordnum == 0) //key word
			{

				if(0 == strcmp((char *)words,"ip"))
				{
					action = CONF_IP;
				}
				if(0 == strcmp((char *)words,"show"))
				{
					action = CONF_SHOW;
				}
				if(0 == strcmp((char *)words,"mqtt-server"))
				{
					action = CONF_MQTT_SERVER;
				}

				if(0 == strcmp((char *)words,"mqtt-port"))
				{
					action = CONF_MQTT_PORT;
				}		
				if(0 == strcmp((char *)words,"reboot"))
				{
					action = CONF_MQTT_REBOOT;
				}
				
				if(0 == strcmp((char *)words,"mac"))
				{
					action = CONF_MAC;
				}

				if(action == CONF_INVALID)
				{
					printf("invalid command!\r\n");
					return;
				}
				wordlen = 0;
			}
			wordnum++;
		}
		else
		{
				words[wordlen] = cmdStr[i];
				wordlen++;
		}
	}

	words[wordlen] = 0;
	
	switch(action)
	{
		case CONF_IP:
			config_IPAddress(words);
			break;	
		case CONF_SHOW:
			config_Show(words);
			break;
		case CONF_MQTT_SERVER:
			config_mqtt(CONF_MQTT_SERVER,words);
			break;		
		case CONF_MQTT_PORT:
			config_mqtt(CONF_MQTT_PORT,words);
			break;	
		case CONF_MAC:
			config_mac(words);
			break;	

		case CONF_MQTT_REBOOT:
			save_rebootReason(REBOOT_BY_CLI);			
			delay(1000);
			NVIC_SystemReset();
			break;		
		default:  //invalid
			printf("invalid command!\r\n");
			break;
	}	

}

void config_task(void *pvParameters)
{
	signed char  count = 0;
	signed char key;


	while(1)
	{	
		if(xSerialGetChar(0, &key , RX_BLOCK_TIME,serCOM2 )!= pdFALSE)  
		{

			if(key == KEY_BACK || key == KEY_DEL)  
			{
				if(count != 0) 	count --; 
				continue;
			}
			else
			{
				RS232_buff[count] = key;				
			}
		
			
			if(key == KEY_ENTER)  
			{
				printf("\r\n");
				RS232_buff[count] = 0;
				if(count !=0)
				{
					config_command((char *)RS232_buff, count);
				}

				count = 0;
				
				printf("\r\n[eIoT]");
				continue;
			}
			count++;
			continue;
		}
		
		vTaskDelay(100);
	}
}



void 	config_Recover(void)
{
	Config_data *pCfg = &config;
	Read_Flash((u32 *)pCfg,sizeof(Config_data)/sizeof(u32));
	
	//如果没有配置，采用默认值
	if(pCfg->ip == 0)  //host ip
	{
		pCfg->ip[0]=10;
		pCfg->ip[1]=1;
		pCfg->ip[2]=0;
		pCfg->ip[3]=32;		
	}
	if(pCfg->mqtt_server_ip == 0)  //mqtt server ip
	{
		pCfg->mqtt_server_ip[0]=10;
		pCfg->mqtt_server_ip[1]=1;
		pCfg->mqtt_server_ip[2]=0;
		pCfg->mqtt_server_ip[3]=200;		
	}	
	if(pCfg->mqtt_server_port == 0)  //mqtt server port
	{
		pCfg->mqtt_server_port = 1883;
	}
	// if no mac, use default
	if(pCfg->mac[0] ==0 && pCfg->mac[1] ==0 && pCfg->mac[2] ==0 &&pCfg->mac[3] ==0 && pCfg->mac[4] ==0 && pCfg->mac[5] ==0)
	{
		pCfg->mac[0] = 0x00;
		pCfg->mac[1] = 0xe0;
		pCfg->mac[2] = 0xfc;
		pCfg->mac[3] = 0x18;
		pCfg->mac[4] = 0x66;
		pCfg->mac[5] = 0x99;
	}
	//恢复上次启动原因
	app_setErrorReason((unsigned char) config.bootReason);
}

u16 config_getVersion(void)
{

	return SOFTWARE_VERSION;
}


void config_getMac(char *mac)
{
	memcpy(mac,config.mac,sizeof(config.mac));
	return;
}


void config_getLocalHostIP(char *ip)
{
	memcpy(ip,config.ip,sizeof(config.ip));
	return;
}

void config_getMqttServerIP(char *ip)
{
	memcpy(ip,config.mqtt_server_ip,sizeof(config.mqtt_server_ip));
	return;
}

u32 config_getMqttServerPort(void)
{
	return config.mqtt_server_port;
}

void save_rebootReason(unsigned int reason)
{

	Config_data data;
	
	//read the old config
	Read_Flash((u32 *)&data,sizeof(Config_data)/sizeof(u32));	

	data.bootReason = reason;
	
	if(1 != Write_Flash((u32 *)&data,sizeof(Config_data)/sizeof(u32)))
	{
		printf("save reason error\r\n");
	}


	return;

}

void configTaskCreate(void)
{

    xTaskCreate(config_task, "conf", 1024, NULL, Config_TASK_PRIO, NULL);
}


