#include "FreeRTOS.h"
#include "timers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "loggerManage.h"

Logger_Data g_logger[MAX_LOGGER_NUM];

void lgm_Addlogger(char * long_add)
{
	int i;
	int bFind=-1;
	int bFree =-1;
	int bMax =-1;
	char bMaxIndex =0;
	char index;
	
	for(i=0;i<MAX_LOGGER_NUM;i++)
	{
		if(g_logger[i].isValid == 0)
		{
			bFree = i;
			continue;
		}

		if(0 == memcmp(long_add,g_logger[i].long_add,LONG_ADD_LEN))
		{
			bFind = i;
			break;
		}
		//find a max order, if the list is full, replace the max;
		if(g_logger[i].order > bMax)
		{
			bMax = g_logger[i].order;
			bMaxIndex = i;
		}
	}

	//找到
	if(bFind != -1)
	{
		g_logger[bFind].order = 0;
		memcpy(g_logger[bFind].long_add,long_add,LONG_ADD_LEN);
	}
	else
	{
		// 如果有空闲
		if(bFree != -1)
		{
			index = (char)bFree;
		}
		else  //find a max order
		{
			index = bMaxIndex;
		}
		g_logger[index].isValid = 1;
		memcpy(g_logger[index].long_add,long_add,LONG_ADD_LEN);
		g_logger[index].order = 0;
	}

	return;
	
}


void lgm_ShowAll(void)
{
	int count = 0;
	int i;

	for(i=0;i<MAX_LOGGER_NUM;i++)
	{
		if(g_logger[i].isValid == 0)
		{
			continue;
		}
		count ++;
		printf("\r\n");		
		printf("index    : %d \r\n",i);
		printf("order    : %d \r\n",g_logger[i].order);
		printf("max addr : %02x:%02x:%02x:%02x:%02x:%02x \r\n",g_logger[i].long_add[0],g_logger[i].long_add[1],g_logger[i].long_add[2],
										g_logger[i].long_add[3],g_logger[i].long_add[4],g_logger[i].long_add[5]);
	}		
	
	printf("Total	 : %d \r\n",count);
	return;
}


//////// TIMER
#define TIMER_LOGGER_ID 0
TimerHandle_t loggerMngTimerHandle;
unsigned int loggerMngTimerCount = 0;



void loggerTimerCallback()
{
	int i;
	loggerMngTimerCount ++;
	if(loggerMngTimerCount >=60)  //every hour
	{
		loggerMngTimerCount = 0;
		for(i=0;i<MAX_LOGGER_NUM;i++)
		{
			if(g_logger[i].isValid == 0)
			{
				continue;
			}			
			if(g_logger[i].order != MAX_ORDER)
			{
				g_logger[i].order++;
			}
		}		
		
	}
}


void initLoggerMng(void)
{
	int i;
	for(i=0;i<MAX_LOGGER_NUM;i++)
	{
		g_logger[i].isValid = 0;
		g_logger[i].order = MAX_ORDER;
	}

	//start a timer, each logger order ++ per 10 min
	loggerMngTimerHandle = xTimerCreate("logger",		   /* 定时器名字 */
							   60000,	 /* 定时器周期,单位时钟节拍 60000 = 1 minites*/
							   pdTRUE,			/* 周期性 */
							   TIMER_LOGGER_ID, 	 /* 定时器ID */
							   loggerTimerCallback); /* 定时器回调函数 */

	
}



