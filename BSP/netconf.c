/**
  ******************************************************************************
  * @file    netconf.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    07-October-2011
  * @brief   Network connection configuration
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/dhcp.h"
#include "netif/ethernetif.h"
#include "FreeRTOS_net_config.h"
#include "netconf.h"
#include "lwip/tcpip.h"
#include <stdio.h>

#define _DEBUG
#include "dprintf.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum 
{ 
  DHCP_START=0,
  DHCP_WAIT_ADDRESS,
  DHCP_ADDRESS_ASSIGNED,
  DHCP_TIMEOUT
} 
DHCP_State_TypeDef;
/* Private define ------------------------------------------------------------*/
#define MAX_DHCP_TRIES 5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct netif xnetif; /* network interface structure */

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
void LwIP_Init(void)
{
  struct ip_addr ipaddr;
  struct ip_addr netmask;
  struct ip_addr gw;
  char ip[4];

  /* Create tcp_ip stack thread */
  tcpip_init( NULL, NULL );	

  /* IP address setting & display on STM32_evalboard LCD*/
#ifdef USE_DHCP
  ipaddr.addr = 0;
  netmask.addr = 0;
  gw.addr = 0;
#else
  config_getLocalHostIP(ip);
  IP4_ADDR(&ipaddr,ip[0],ip[1],ip[2],ip[3]);
  IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3); //固定为255.255.255.1
  IP4_ADDR(&gw, ip[0], ip[1], ip[2], 1); //gateway 默认为1
#endif

  /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
            struct ip_addr *netmask, struct ip_addr *gw,
            void *state, err_t (* init)(struct netif *netif),
            err_t (* input)(struct pbuf *p, struct netif *netif))
    
   Adds your network interface to the netif_list. Allocate a struct
  netif and pass a pointer to this structure as the first argument.
  Give pointers to cleared ip_addr structures when using DHCP,
  or fill them with sane numbers otherwise. The state pointer may be NULL.

  The init function pointer must point to a initialization function for
  your ethernet netif interface. The following code illustrates it's use.*/

  netif_add(&xnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

 /*  Registers the default network interface. */
  netif_set_default(&xnetif);

 /*  When the netif is fully configured this function must be called.*/
  netif_set_up(&xnetif); 
}


#ifdef USE_DHCP
/**
  * @brief  LwIP_DHCP_Process_Handle
  * @param  None
  * @retval None
  */
int g_DHCP_OK_flag = 0;

void LwIP_DHCP_task(void * pvParameters)
{
  struct ip_addr ipaddr;
  struct ip_addr netmask;
  struct ip_addr gw;
  uint32_t IPaddress;
  uint8_t iptab[4];
  uint8_t iptxt[20];
  uint8_t DHCP_state;  
  DHCP_state = DHCP_START;

  for (;;)
  {
    switch (DHCP_state)
    {
      case DHCP_START:
	      {
	        dhcp_start(&xnetif);
	        IPaddress = 0;
	        DHCP_state = DHCP_WAIT_ADDRESS;
					printf("Looking for DHCP server,please wait... \r\n");
	      }
	      break;

      case DHCP_WAIT_ADDRESS:
	      {
	        /* Read the new IP address */
	        IPaddress = xnetif.ip_addr.addr;
	
	        if (IPaddress!=0) 
	        {
	          DHCP_state = DHCP_ADDRESS_ASSIGNED;	
	          
	          /* Stop DHCP */
	          dhcp_stop(&xnetif);
	
	   
	          iptab[0] = (uint8_t)(IPaddress >> 24);
	          iptab[1] = (uint8_t)(IPaddress >> 16);
	          iptab[2] = (uint8_t)(IPaddress >> 8);
	          iptab[3] = (uint8_t)(IPaddress);
	
	          sprintf((char*)iptxt, "  %d.%d.%d.%d", iptab[3], iptab[2], iptab[1], iptab[0]);  
	          /* end of DHCP process: LED1 stays ON*/
	          //STM_EVAL_LEDOn(LED1);//....
						printf("\r\nIP address[%s ]assigned by a DHCP server !\r\n",iptxt);
						extern void testTask(void);
						testTask();
	          vTaskDelete(NULL);
	        }
	        else
	        {
	          /* DHCP timeout */
	          if (xnetif.dhcp->tries > MAX_DHCP_TRIES)
	          {
	            DHCP_state = DHCP_TIMEOUT;
	
	            /* Stop DHCP */
	            dhcp_stop(&xnetif);
	
	            /* Static address used */
	            IP4_ADDR(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
	            IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
	            IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	            netif_set_addr(&xnetif, &ipaddr , &netmask, &gw);
	
	            iptab[0] = IP_ADDR3;
	            iptab[1] = IP_ADDR2;
	            iptab[2] = IP_ADDR1;
	            iptab[3] = IP_ADDR0;
	
	            sprintf((char*)iptxt, "  %d.%d.%d.%d", iptab[3], iptab[2], iptab[1], iptab[0]); 
	
	            /* end of DHCP process: LED1 stays ON*/
	            //STM_EVAL_LEDOn(LED1);//....
							printf("\r\nDHCP timeout    \r\nStatic IP address   :%s\r\n",iptxt);
	            vTaskDelete(NULL);
	          }
						{
							static int s=0;
							const char tokens[4]={'-','\\','|','/'};
							printf("\b\b\b\b%c%c%c%c",tokens[s++%4],tokens[s%4],tokens[s%4],tokens[s%4]);
						}
	        }
	      }
	      break;

      default:
				printf("\r\nerr: DHCP_state=%d\r\n",DHCP_state); 
				break;
    }

    /* Toggle LED1 */
    //STM_EVAL_LEDToggle(LED1);//...
    /* wait 250 ms */
    vTaskDelay(250);
  }   
}
#endif  /* USE_DHCP */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
