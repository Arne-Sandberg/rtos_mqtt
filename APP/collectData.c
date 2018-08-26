#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
//#include "MQTTClient.h"
#include <stdio.h>
#include <stdlib.h>

#include "serial.h"
#include <string.h>

#include "i2c_slave.h"
#include "MqttTask.h"

#include "collectData.h"
#include "loggerManage.h"



void collectData_task(void *pvParameters)
{

	I2cBuff *pBuf = 0;

	char i;
	unsigned char logger_add[LONG_ADD_LEN];
	unsigned int batt=0;
	
	while(1)
	{	
		pBuf = i2c_get_buf();
	
		// check the queue if i2c get data -----------------
		while( pBuf != 0 )
		{

			//借用标记位，表示已经读取
			if(pBuf->stop == 1)
			{
				pBuf->stop = 0;
				
				printf("\n reg: %d data:",pBuf->reg);

				for(i=0;i<pBuf->len;i++)
				{
					printf("%02x ",pBuf->data[i]);
				}

				switch (pBuf->reg)
				{
					case LOGGER_DATA: //来自LOGGER的传感数据，温度湿度和红外
						//4-9 为MAC
						memcpy((void *)logger_add,(pBuf->data)+3,LONG_ADD_LEN);
						//1-2字节是温湿度电池电量
						batt = pBuf->data[10]*256+pBuf->data[9]; //字节序转换 
						mqtt_SensorDataCb(pBuf->data[0], pBuf->data[1],pBuf->data[2],batt,logger_add); 
						//3字节为IR
					//	mqtt_IRDataCb(pBuf->data[3],logger_add);
						//增加设备
						lgm_Addlogger((char *)logger_add);
					//	vTaskDelay(1000);

						mqtt_ReportDataCb(logger_add);
					break;

					
					case GW_GPIO_DATA: //来自网关的传感数据

						mqtt_GwSensorDataCb(pBuf->data[0],pBuf->data[1]);
					break;
				}

				
				
#if 0				
				switch (pBuf->reg)
				{
					case TH_IR_DATA:
					//	pp[0] = pBuf->data[2];
					//	pp[1] = pBuf->data[3];
						memcpy((void *)logger_add,(pBuf->data)+2,LONG_ADD_LEN);
						mqtt_THDataCb(pBuf->data[0], pBuf->data[1],logger_add);						
					break;
					case IRDATA:
						memcpy((void *)logger_add,(pBuf->data)+1,LONG_ADD_LEN);				
						mqtt_IRDataCb(pBuf->data[0],logger_add);
					break;
					case LOGGER_REPORT:
			
						//mqtt_IRDataCb(pBuf->data[0],logger_add);
						//NEED LOGGER MANAGER MENAGE
						lgm_Addlogger((char *)pBuf->data);
						mqtt_ReportDataCb(logger_add);
					break;
				

				}
#endif	
			}

			pBuf = i2c_getNext_buf(); 
	
		}

		
		vTaskDelay(100);
	}
}


void collectTaskCreate(void)
{
	xTaskCreate(collectData_task, "coll", 1024, NULL, Collect_TASK_PRIO, NULL);

	
}

