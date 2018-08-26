
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FREERTOS_NET_CONFIG_H__
#define __FREERTOS_NET_CONFIG_H__

#ifdef __cplusplus
 extern "C" {
#endif


/* MAC ADDRESS*/
#define MAC_ADDR0   0x00
#define MAC_ADDR1   0x80
#define MAC_ADDR2   0xe1
#define MAC_ADDR3   0x31
#define MAC_ADDR4   0x45
#define MAC_ADDR5   0x57
 
/*Static IP ADDRESS*/
#define IP_ADDR0   10
#define IP_ADDR1   1
#define IP_ADDR2   0
#define IP_ADDR3   30
   
/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/*Gateway Address*/
#define GW_ADDR0   10
#define GW_ADDR1   1
#define GW_ADDR2   0
#define GW_ADDR3   1  


#ifdef __cplusplus
}
#endif

#endif 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

