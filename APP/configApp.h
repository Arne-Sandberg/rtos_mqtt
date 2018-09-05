
#ifndef __CONFIG_APP_C_
#define __CONFIG_APP_C_

#define Config_TASK_PRIO    ( tskIDLE_PRIORITY+3 )
#define RS232_REC_BUFF_SIZE 256
#define KEY_SPACE 0x20
#define KEY_ENTER 0x0d
#define KEY_BACK  0x08
#define KEY_DEL  0x7f

#define RX_BLOCK_TIME			( ( TickType_t ) 0xffff )

#define TRUE 1

#define CONFIG_FLASH_ADDRESS    ((u32)0x08030000)


enum configAction
{
	CONF_INVALID = 0,
	CONF_IP = 1,
	CONF_MAC,
	CONF_SHOW,
	CONF_MQTT_SERVER,
	CONF_MQTT_PORT,	
	CONF_MQTT_REBOOT,
};

//需要4字节对齐
typedef struct
{
	unsigned char ip[4];
	unsigned char mqtt_server_ip[4];
	unsigned long  mqtt_server_port;
	unsigned char mac[6];
	unsigned int version;
	unsigned long bootReason;
	
}  Config_data;                       

extern void config_Recover(void);
extern void config_getMac(char *mac);
extern u16 config_getVersion(void);
extern void config_getLocalHostIP(char *ip);
extern u32 config_getMqttServerPort(void);
extern void config_getMqttServerIP(char *ip);




#endif
