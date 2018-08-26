#ifndef MQTTFREERTOSIMPL_H
#define MQTTFREERTOSIMPL_H

#include "FreeRTOS.h"
#include "portmacro.h"

typedef struct Timer Timer;

struct Timer
{
    portTickType end_time;
};

typedef struct Network Network;

struct Network
{
	int my_socket;
	int (*mqttread) (Network*, unsigned char*, int, int);
	int (*mqttwrite) (Network*, unsigned char*, int, int);
};

char expired(Timer*);
void countdown_ms(Timer*, unsigned int);
void countdown(Timer*, unsigned int);
int left_ms(Timer*);

void InitTimer(Timer*);

int FreeRTOS_MQTT_read(Network*, unsigned char*, int, int);
int FreeRTOS_MQTT_write(Network*, unsigned char*, int, int);
void FreeRTOS_MQTT_disconnect(Network*);

void NewNetwork(Network* n);
int ConnectNetwork(Network* n, const char* host, int port);
int DisconnectNetwork(Network* n);


#endif /* MQTTFREERTOSIMPL_H */
