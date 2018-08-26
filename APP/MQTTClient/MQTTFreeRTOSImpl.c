
/*
²Î¿¼https://github.com/baoshi/ESP-RTOS-Paho/tree/63c2c74dfe978f215b3bb05f7e1258454908c4fb
*/

#include <string.h>  
#include "FreeRTOS.h"
#include "portmacro.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sys.h"

#include "MQTTFreeRTOSImpl.h"

#define _DEBUG
#include "dprintf.h"

char  expired(Timer* timer)
{
    portTickType now = xTaskGetTickCount();
	int32_t left = timer->end_time - now;
	return (left < 0);
}


void  countdown_ms(Timer* timer, unsigned int timeout)
{
    portTickType now = xTaskGetTickCount();
	timer->end_time = now + timeout / portTICK_RATE_MS;
}


void  countdown(Timer* timer, unsigned int timeout)
{
    countdown_ms(timer, timeout);
}


int  left_ms(Timer* timer)
{
    portTickType now = xTaskGetTickCount();
    int32_t left = timer->end_time - now;
	return (left < 0) ? 0 : left / portTICK_RATE_MS;
}


void  InitTimer(Timer* timer)
{
	timer->end_time = 0;
}

#include "task.h"
#include "timers.h"

__asm void _nop(void)
{	
	nop		
}
void delay(int i)
{
	int j;
	for(;i>0;i--)
		j++;
		//_nop();
}

int  FreeRTOS_MQTT_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	TickType_t xTicksToWait = timeout_ms / portTICK_PERIOD_MS; /* convert milliseconds to ticks */
	TimeOut_t xTimeOut;
	int recvLen = 0;

	vTaskSetTimeOutState(&xTimeOut); /* Record the time at which this function was entered. */
	do
	{
		int rc = 0;
		lwip_setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, &xTicksToWait, sizeof(TickType_t));
		rc = recv(n->my_socket, buffer + recvLen, len - recvLen, 0);
		//dprintf("rc=%d\n",rc);
		delay(50);//must delay enough,otherwise,it will be  blocked
		if (rc >= 0)
			recvLen += rc;	
		else if (rc <0)
		{	
			recvLen = rc;
			break;
		}
	} while (recvLen < len && xTaskCheckForTimeOut(&xTimeOut, &xTicksToWait) == pdFALSE);

	return recvLen;
}


int  FreeRTOS_MQTT_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	TickType_t xTicksToWait = timeout_ms / portTICK_PERIOD_MS; /* convert milliseconds to ticks */
	TimeOut_t xTimeOut;
	int sentLen = 0;

	vTaskSetTimeOutState(&xTimeOut); /* Record the time at which this function was entered. */
	do
	{
		int rc = 0;

		lwip_setsockopt(n->my_socket, SOL_SOCKET, SO_SNDTIMEO, &xTicksToWait, sizeof(xTicksToWait));
		rc = send(n->my_socket, buffer + sentLen, len - sentLen, 0);
		if (rc > 0)
			sentLen += rc;
		else if (rc < 0)
		{
			sentLen = rc;
			break;
		}
	} while (sentLen < len && xTaskCheckForTimeOut(&xTimeOut, &xTicksToWait) == pdFALSE);

	return sentLen;
}



void  NewNetwork(Network* n)
{
	n->my_socket = -1;
	n->mqttread = FreeRTOS_MQTT_read;
	n->mqttwrite = FreeRTOS_MQTT_write;
}


int  host2addr(const char *hostname , struct in_addr *in)
{
    struct addrinfo hints;
	struct addrinfo*servinfo, *p;
    struct sockaddr_in *h;
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    rv = getaddrinfo(hostname, 0 , &hints , &servinfo);
    if (rv != 0)
    {
        return rv;
    }

    // loop through all the results and get the first resolve
    for (p = servinfo; p != 0; p = p->ai_next)
    {
        h = (struct sockaddr_in *)p->ai_addr;
        in->s_addr = h->sin_addr.s_addr;
    }
    freeaddrinfo(servinfo); // all done with this structure
    return 0;
}


int  ConnectNetwork(Network* n, const char* host, int port)
{
    struct sockaddr_in addr;
    int ret;

    if (host2addr(host, &(addr.sin_addr)) != 0)
    {
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    n->my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( n->my_socket < 0 )
    {
        // error
        return -1;
    }
    ret = connect(n->my_socket, ( struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if( ret < 0 )
    {
        // error
        lwip_close(n->my_socket);
        return ret;
    }

    return ret;
}


int  DisconnectNetwork(Network* n)
{
    lwip_close(n->my_socket);
    n->my_socket = -1;
    return 0;
}

