#ifndef _COMMON_H_
#define _COMMON_H_  1

#define FI 1
#define DI 2

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

#endif

