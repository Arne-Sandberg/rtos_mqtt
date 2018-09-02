
#include "FreeRTOS.h"
#include "stm32f10x.h"

#include "task.h"
#include "queue.h"
#include "timers.h"
#include "configApp.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

//#define _DEBUG
#include "dprintf.h"
#include "MqttTask.h"

void reportTimerCallback(xTimerHandle pxTimer);

void  topic_received(MessageData* md)
{
    int i;
    MQTTMessage* message = md->message;
    printf("Received Topic[");
    for (i = 0; i < md->topic->lenstring.len; ++i)
        printf("%c",md->topic->lenstring.data[i]);
    printf("], Message[ ");
    for (i = 0; i < (int)message->payloadlen; ++i)
        printf("%c",((char*)message->payload)[i]);
    printf("]\n");
}



xQueueHandle publish_queue;

TimerHandle_t reportTimerHandle;

unsigned int g_timer = MQTT_REPORT_PEROID;

static MQTTClient client = DefaultClient;

u16 g_reportPacketStat = 0;




#define PUB_MSG_LEN 16

#include "common.h"

unsigned char g_mqttConnect = 1;

void mqtt_task(void *pvParameters)
{
    int ret;
    struct Network network;
    //char mqtt_client_id[20];
    unsigned char mqtt_buf[128];
    unsigned char mqtt_readbuf[128];
	char strIP[32];
	char ip[4];

	unsigned int port;

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

	/*start report peroid timer*/
    const TickType_t  xTimerPer = 60000;  //tick = 1ms, 60000=1 分钟
	
        reportTimerHandle = xTimerCreate("Timer",          /* 定时器名字 */
                                   xTimerPer,    /* 定时器周期,单位时钟节拍 */
                                   pdTRUE,          /* 周期性 */
                                   TIMER_REPORT_ID,      /* 定时器ID */
                                   reportTimerCallback); /* 定时器回调函数 */

        if(reportTimerHandle == NULL)
        {
            /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
        }
        else
        {
             /* 启动定时器，系统启动后才开始工作 */
             if(xTimerStart(reportTimerHandle, 10) != pdPASS)//等待延时100ms，其实设置成0在简单任务下也是可以的，这个数值根据项目需求更改
             {
                 /* 定时器还没有进入激活状态 */
             }
        }
  

    NewNetwork(&network);
    while (1)
    {
		if(g_mqttConnect)
		{

			#ifndef USR_SD_CARD_INI
				config_getMqttServerIP(ip);
				port = config_getMqttServerPort();
		
				sprintf(strIP,"%d.%d.%d.%d",ip[0],ip[1],ip[2],ip[3]);
				
		        printf("(Re)connecting to MQTT server %s ... ", strIP);
		        ret = ConnectNetwork(&network, strIP, port);
			#else
					if(InitMQTTServerInfo()<0)
						continue;
	       			printf("(Re)connecting to MQTT server %s:%d ... ", getMQTTServerIP(),getMQTTServerPort());
			        ret = ConnectNetwork(&network, getMQTTServerIP(), getMQTTServerPort());
			#endif
	        if (!ret)
	        {
	            printf("ok.\r\n");
	            NewMQTTClient(&client, &network, 1000, mqtt_buf, 128, mqtt_readbuf, 128);
	            data.willFlag = 0;
	            data.MQTTVersion = 3;
	            data.clientID.cstring = mqtt_client_id;
						#ifndef USR_SD_CARD_INI
	            data.username.cstring = MQTT_USER;
	            data.password.cstring = MQTT_PASS;					
						#else
	            data.username.cstring = getMQTTServerName();
	            data.password.cstring = getMQTTServerPassword();
						#endif
	            data.keepAliveInterval = 60;
	            data.cleansession = 1;
	            printf("Send MQTT connect ...");
	            ret = MQTTConnect(&client, &data);
	            if (!ret)
	            {
	                printf("ok.\r\n");
	                // Subscriptions
	                MQTTSubscribe(&client, SUB_TOPIC_1, QOS1, topic_received);
	                // Empty the publish queue
	                xQueueReset(publish_queue);dprintf("\n");
	                while (1)
	                {	
						dprintf("\n");
	                    // Receiving / Ping
	                    ret = MQTTYield(&client, 1000);
											dprintf("\n");
	                    if (ret == DISCONNECTED)
	                    {  
							dprintf("DISCONNECTED\n");
	                        break;
	                    }
	                }
	                printf("Connection broken, request restart\r\n");
	            }
	            else
	            {
	                printf("failed.\r\n");
	            }
	            DisconnectNetwork(&network);dprintf("\n");
	        }
	        else
	        {
	            printf("failed.\r\n");
	        }
		}
		

        vTaskDelay(100);
    }
  //  vTaskDelete(NULL);
}

void mqttTaskCreate(void)
{
    publish_queue = xQueueCreate(3, PUB_MSG_LEN);
    xTaskCreate(mqtt_task, "mqtt", 1024, NULL, MQTTClient_TASK_PRIO, NULL);
	/* Create the queues used to hold Rx/Tx characters. */
//	xNotifyDataFromI2c = xQueueCreate( 1, ( unsigned portBASE_TYPE ) sizeof( signed char ) );

	
}



//上报LOGGER心跳
void  mqtt_ReportDataCb(unsigned char * log_add)
{
	char source[6];
	u8 length = 0;
	char buf[128] = {0};
	MQTTMessage msg;


	config_getMac(source);

	sprintf(buf+length,"{esn:'"MACSTR"',",MAC2STR(log_add));
	length = strlen(buf);
	


	sprintf(buf+length,"mac:'"MACSTR"',",MAC2STR(log_add));
	length = strlen(buf);
	
	sprintf(buf+length,"type:1,");
	length = strlen(buf);	


	sprintf(buf+length,"gateway:'"MACSTR"'}",MAC2STR(source));
	length = strlen(buf);	

	
	msg.qos = QOS0;
	msg.retained = 0;
	msg.payload = buf;
	msg.payloadlen = length;
	
	if(MQTT_SUCCESS ==  MQTTPublish(&client, "gateway_report", &msg))
	{
		g_reportPacketStat++;
		printf("send sensorData --> %d\r\n",g_reportPacketStat);
	}
	else
	{
		printf("send sensorData fail!");
	}


}


//上报温湿度传感器数据
void  mqtt_IRDataCb(unsigned char ir,unsigned char * log_add)
{
	//char source[6];
	u8 length = 0;
	char buf[128] = {0};
	MQTTMessage msg;


//	config_getMac(source);

	//last add use log id
	//memcpy(source+1,&log_add,2);
	sprintf(buf+length,"{mac:'"MACSTR"',",MAC2STR(log_add));
	length = strlen(buf);


	sprintf(buf+length,"ir:%d}",ir);
	length = strlen(buf);	


	msg.qos = QOS0;
	msg.retained = 0;
	msg.payload = buf;
	msg.payloadlen = length;
	
	if(MQTT_SUCCESS ==  MQTTPublish(&client, "sensorData", &msg))
	{
		g_reportPacketStat++;
		printf("send sensorData --> %d\r\n",g_reportPacketStat);
	}
	else
	{
		printf("send sensorData fail!");
	}


}


//上报温湿度传感器数据
void  mqtt_SensorDataCb(unsigned char temp, unsigned char hum, unsigned char ir,unsigned int batt,unsigned char * log_add)
{
	//char source[6];
	u8 length = 0;
	char buf[128] = {0};
	float bb = (float)batt/100;
	MQTTMessage msg;


//	config_getMac(source);
//	memcpy(source+1,&log_add,2);

	
	sprintf(buf+length,"{mac:'"MACSTR"',",MAC2STR(log_add));
	length = strlen(buf);


	sprintf(buf+length,"temperature:%d,humidity:%d,ir:%d,battery:%.2f}",temp,hum,ir,bb);
	length = strlen(buf);	
	if(length >= 128)
	{
		printf("buffer overflow!");
		return;
	}

	msg.qos = QOS0;
	msg.retained = 0;
	msg.payload = buf;
	msg.payloadlen = length;
	
	if(MQTT_SUCCESS ==  MQTTPublish(&client, "sensorData", &msg))
	{
		g_reportPacketStat++;
		printf("send sensorData --> %d\r\n",g_reportPacketStat);
	}
	else
	{
		printf("send sensorData fail!");
	}
	
	return;


}

//WATER and SMOKE
void  mqtt_GwSensorDataCb(unsigned char water, unsigned char smoke)
{
	char source[6];
	u8 length = 0;
	char buf[128] = {0};

	MQTTMessage msg;

	if(water ==0  && smoke ==0)
	{
		return;
	}

	config_getMac(source);
//	memcpy(source+1,&log_add,2);

	
	sprintf(buf+length,"{mac:'"MACSTR"'",MAC2STR(source));
	length = strlen(buf);

	if(water != 0)
	{
		sprintf(buf+length,",water:%d",water);
		length = strlen(buf);	
	}


	if(smoke != 0)
	{
		sprintf(buf+length,",smoke:%d",smoke);
		length = strlen(buf);	
	}
	sprintf(buf+length,"}");
	length = strlen(buf);	

	//length++;
	//buf[length+1]='}';

	if(length >= 128)
	{
		printf("buffer overflow!");
		return;
	}

	msg.qos = QOS0;
	msg.retained = 0;
	msg.payload = buf;
	msg.payloadlen = length;
	
	if(MQTT_SUCCESS ==  MQTTPublish(&client, "sensorData", &msg))
	{
		g_reportPacketStat++;
		printf("send sensorData --> %d\r\n",g_reportPacketStat);
	}
	else
	{
		printf("send sensorData fail!");
	}
	
	return;


}


void  mqtt_reportCb(MQTTClient * client)
{
	char source[6];
	u8 length = 0;
	char buf[128] = {0};
	char ip[4];
	MQTTMessage msg;


	u32 chipID_low,chipID_high;

	
	chipID_low = *(__IO u32*)(0X1FFFF7E8);
	chipID_high =  *(__IO u32 *)(0X1FFFF7EC);

	config_getMac(source);
	config_getLocalHostIP(ip);


	sprintf(buf+length,"{esn:'%08X%08X',",chipID_high,chipID_low);
	length = strlen(buf);
	
	sprintf(buf+length,"mac:'"MACSTR"',",MAC2STR(source));
	length = strlen(buf);

	sprintf(buf+length,"ip:'""%d.%d.%d.%d""',",ip[0],ip[1],ip[2],ip[3]);
	length = strlen(buf);

	sprintf(buf+length,"type:0,");
	length = strlen(buf);

	sprintf(buf+length,"version:%d}",config_getVersion());
	length = strlen(buf);	


	msg.qos = QOS0;
	msg.retained = 0;
	msg.payload = buf;
	msg.payloadlen = length;
	
	if(MQTT_SUCCESS ==  MQTTPublish(client, "gateway_report", &msg))
	{
		g_reportPacketStat++;
		printf("send peroid report--> %d\r\n",g_reportPacketStat);
	}
	else
	{
		printf("send peroid report fail!");
	}


}

static void reportTimerCallback(xTimerHandle pxTimer)
{
	g_timer --;
	if(g_timer == 0)
	{
		g_timer = MQTT_REPORT_PEROID;
		mqtt_reportCb(&client);
	}

	#if 0 
    uint32_t ulTimerID;

    configASSERT(pxTimer);

    /* 获取那个定时器时间到 */
    ulTimerID = (uint32_t)pvTimerGetTimerID(pxTimer);
    
    /* 处理定时器0任务 */
    if(ulTimerID == 0)
    {
        LED2_TOGGLE;
    }
    
    /* 处理定时器1任务 */
    if(ulTimerID == 1)
    {
        BEEP_TOGGLE;
    }
	#endif
}



