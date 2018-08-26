
#ifndef __MQTT_TASK_C_
#define __MQTT_TASK_C_


#include "MQTTClient.h"
#define MQTT_USER ""
#define MQTT_PASS ""
#define mqtt_client_id "eIot Gateway"

#define MQTT_REPORT_PEROID 1

#define SUB_TOPIC_1	"FCS/out"

#define MQTTClient_TASK_PRIO    ( tskIDLE_PRIORITY+2 )





void  mqtt_SensorDataCb(unsigned char temp, unsigned char hum ,unsigned char ir,unsigned int batt,unsigned char *log_add);
void  mqtt_IRDataCb(unsigned char ir,unsigned char * log_add);

void  mqtt_GwSensorDataCb(unsigned char water, unsigned char smoke);




#endif

