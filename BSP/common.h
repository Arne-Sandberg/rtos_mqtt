#ifndef _COMMON_H_
#define _COMMON_H_  1

#define FI 1
#define DI 2

#define TIMER_REPORT_ID 0
#define TIMER_WATHDOG_ID 1

typedef enum
{
    REBOOT_INVALID, 
	REBOOT_BY_POWER,	
	REBOOT_BY_CLI,
	REBOOT_BY_HARDWARE,		
	REBOOT_BY_WATCHDOG,
	REBOOT_UNKNOW,	
}APP_REBOOT_REASON;



void Sys_Soft_Reset(void);
void get_file_name(uint8_t *file_name,uint8_t length,uint8_t type);
uint8_t check_file_name(uint8_t *file_name,uint8_t length);

void format_disk(void);
void creat_file(void);
void delete_file(void);
void list_file(void);
void get_disk_info(void);
void creat_dir(void);
void edit_file(void);
void read_file(void);


extern int InitMQTTServerInfo(void);
extern const char * getMQTTServerIP(void); 
extern int getMQTTServerPort(void);
extern char * getMQTTServerName(void);
extern char * getMQTTServerPassword(void);
extern void app_setErrorReason(unsigned char reason);
extern unsigned char app_getErrorReason(void);
extern void save_rebootReason(unsigned int reason);
extern  void delay(int i);
#endif

