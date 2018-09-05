#ifndef __PRINTF_UTILS_H__
#define __PRINTF_UTILS_H__


#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif


extern void FreeRTOS_printf_service_init(void);
extern const char *getCurrentFileName(const char* strFileName);

extern unsigned char  UTILS_IsIPV4 (char *str);
extern unsigned char  UTILS_StrToIP(char* str, void *ip);
extern unsigned long  UTILS_Atoh(const char *s);
extern void UTILS_StrToMac(const char * szMac,unsigned char * pMac);  

#endif
