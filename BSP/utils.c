#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#include "stm32f10x.h"

#include "FreeRTOS.h"
#include "serial.h"

#define FreeRTOS_printf_BAUD_RATE		( 115200 )
#define FreeRTOS_printf_BUFFER_LEN 		 (128)

/* Handle to the com port used by both tasks. */					
static xComPortHandle xPort = NULL;

void FreeRTOS_printf_service_init(void)
{
	xPort = xSerialPortInitMinimal( FreeRTOS_printf_BAUD_RATE, FreeRTOS_printf_BUFFER_LEN );
}

/* 发送数据，如果发送的是退格键。
为了使接收端显示为删除前面字符的效果，则先退格再显示空格，然后再退格。
否则的话，直接退格，字符不能删除，仍然能在串口终端显示*/
int fputc(int ch, FILE *f)
{
	if(ch=='\n')
	{
			USART_SendData(USART2, '\r');// USART1 可以换成 USART2 等
				while (!(USART2->SR & USART_FLAG_TXE));
	}
	else if(ch=='\b')
	{
			USART_SendData(USART2, '\b');// USART1 可以换成 USART2 等
				while (!(USART2->SR & USART_FLAG_TXE));
			USART_SendData(USART2, ' ');// USART1 可以换成 USART2 等
				while (!(USART2->SR & USART_FLAG_TXE));					
	}
	else if(ch=='\r')
	{
		return (ch);
	}
	USART_SendData(USART2, (unsigned char) ch);// USART1 可以换成 USART2 等
	while (!(USART2->SR & USART_FLAG_TXE));
	return (ch);
}

#include <string.h>
//用来获取__FILE__中的不带路径的文件名
const char *getCurrentFileName(const char* strFileName)
{
	const char *p = strrchr(strFileName,'\\');
	if(p==NULL)
		return strFileName;
	return ++p;
}

#include <stdio.h>  

/* _exit - Simple implementation. Does not return.
*/
void _exit (int status)
{
  (void)status;
  while (1);
}

/*
 * abort -- go out via exit...
 */
void abort(void)
{
  _exit(1);
}


void _assert(const char *mesg, const char *file, int line)
{
   printf("%s, %s, %d\n", mesg, file, line); 
   abort();
}




unsigned char  UTILS_IsIPV4 (char *str)
{
	unsigned char segs = 0;   /* Segment count. */
	unsigned char chcnt = 0;  /* Character count within segment. */
	unsigned char accum = 0;  /* Accumulator for segment. */
    /* Catch NULL pointer. */
    if (str == 0)
        return 0;
    /* Process every character in string. */

    while (*str != '\0') {
        /* Segment changeover. */

        if (*str == '.') {
            /* Must have some digits in segment. */
            if (chcnt == 0)
                return 0;
            /* Limit number of segments. */
            if (++segs == 4)
                return 0;
            /* Reset segment values and restart loop. */
            chcnt = accum = 0;
            str++;
            continue;
        }

        /* Check numeric. */
        if ((*str < '0') || (*str > '9'))
            return 0;

        /* Accumulate and check segment. */

        if ((accum = accum * 10 + *str - '0') > 255)
            return 0;
        /* Advance other segment specific stuff and continue loop. */

        chcnt++;
        str++;
    }

    /* Check enough segments and enough characters in last segment. */

    if (segs != 3)
        return 0;
    if (chcnt == 0)
        return 0;
    /* Address okay. */

    return 1;
}

unsigned char  UTILS_StrToIP(char* str, void *ip)
{

	    /* The count of the number of bytes processed. */
	    int i;
	    /* A pointer to the next digit to process. */
	    char * start;

	    start = str;
	    for (i = 0; i < 4; i++) {
	        /* The digit being processed. */
	        char c;
	        /* The value of this byte. */
	        int n = 0;
	        while (1) {
	            c = * start;
	            start++;
	            if (c >= '0' && c <= '9') {
	                n *= 10;
	                n += c - '0';
	            }
	            /* We insist on stopping at "." if we are still parsing
	               the first, second, or third numbers. If we have reached
	               the end of the numbers, we will allow any character. */
	            else if ((i < 3 && c == '.') || i == 3) {
	                break;
	            }
	            else {
	                return 0;
	            }
	        }
	        if (n >= 256) {
	            return 0;
	        }
	        ((char*)ip)[i] = n;
	    }
	    return 1;

}

unsigned long  UTILS_Atoh(const char *s)
{
	unsigned long  value = 0, digit;
	char c;
	
	while((c = *s++)){
		if('0' <= c && c <= '9')
			digit = c - '0';
		else if('A' <= c && c <= 'F')
			digit = c - 'A' + 10;
		else if('a' <= c && c<= 'f')
			digit = c - 'a' + 10;
		else break;
		
		value = (value << 4) | digit;
	}
	
	return value;
}


