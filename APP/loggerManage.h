

#ifndef __LOGGER_MNG_C_
#define __LOGGER_MNG_C_


#define loggerMng_TASK_PRIO    ( tskIDLE_PRIORITY+1 )

#define LONG_ADD_LEN 6 //先按照六位设计
#define MAX_LOGGER_NUM 16 //先按照六位设计
#define MAX_ORDER 0xffff

typedef struct

{

	unsigned int order; 
	char long_add[LONG_ADD_LEN];
	char isValid;
}  Logger_Data; 


void lgm_Addlogger(char * long_add);
void lgm_ShowAll(void);
void initLoggerMng(void);


#endif
