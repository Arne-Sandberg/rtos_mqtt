#include "spi.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include "ff.h"
#include "integer.h"
#include "diskio.h"
#include <string.h>
#include "FreeRTOS.h"
#include "serial.h"

//#define _DEBUG
#include "dprintf.h"

/* Handle to the com port used by both tasks. */
static xComPortHandle xPort = NULL;

/* The Rx task will block on the Rx queue for a long period. */
#define comRX_BLOCK_TIME			( ( TickType_t ) 0xffff )



void edit_file(void)
{
	FATFS fs;
	FIL	file;
	FRESULT res; 
	DIR dirs;
	FILINFO finfo;
	signed char key = 0;
	char path[20];

	uint32_t index = 0x00;
	uint32_t reindex = 0x00;
	uint8_t file_buff[512] = {0};

	uint32_t files_num = 0;
	uint8_t length;
	res = f_mount(&fs,"",0);
	if (res != FR_OK)
	{
		printf("\r\n�����ļ�ϵͳʧ��,�������: %u",res);
		return;
	}
	res = f_opendir(&dirs,"/");
	printf("\r\n------------�ļ��б�------------");
	if (res == FR_OK) 
	{
		while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) 
		{
			if (finfo.fattrib & AM_DIR)//������ļ��� 
			{ 
				continue;
			} 
			else 
			{   
				files_num++;
				//��ʾ�ļ���,��ʾ�ļ�ʵ�ʴ�С ,�ļ�ʵ�ʴ�С�����������뷨
				printf("\r\n/%12s%7ld KB ",  &finfo.fname[0],(finfo.fsize+512)/1024);
			}
		}
		if( files_num == 0 )//���ļ�
		{
			printf("\r\n���ļ�!");    
		}
	}
	else
	{
		printf("\r\n�򿪸�Ŀ¼ʧ��!");
		printf("\r\n�������: %u",res);
	}
	printf("\r\n����Ҫ�༭�ļ�ȫ��,�Իس�����...");
	get_file_name((uint8_t *)path,length,FI);
	res = f_open(&file,path,FA_READ | FA_WRITE);
	if (res == FR_OK)
	{	
		printf("\r\n���ļ� %s �ɹ�",path);
		printf("\r\n�������ļ��༭״̬,������Ҫд�������!");		
		printf("\r\n��ESC����Ctrl+C�����༭!\r\n");
		
		while(1)
		{		
			if(xSerialGetChar( xPort, &key, comRX_BLOCK_TIME,serCOM2 )== pdFALSE)  
			{
				continue;
			}

			if ((key == 0x1B) && (index == 0x00))//key ESC
			{
				printf("\r\n���ݻ�û������,���ڴ��ڱ༭ģʽ...");								
				continue;
			}
			else if ((key == 0x1B))	//key ESC
			{
				printf("\r\n��������...");
				res = f_write(&file,file_buff,index,&reindex);
				if ((res == FR_OK) && (reindex == index))
				{
					printf("\r\n�������ݳɹ�!");
					f_close(&file);
					index = 0x00;
					reindex = 0x00;									
				}
				else
				{
					printf("\r\n��������ʧ��!");
					printf("\r\n�������: %u",res);									
				}
				break;
			}
			else if (key == 0x03) //key Ctrl+C
			{
				printf("\r\n�����ļ��༭!");
				printf("\r\n��������...");
				res = f_write(&file,file_buff,index,&reindex);
				if ((res == FR_OK) && (reindex == index))
				{
					printf("\r\n�������ݳɹ�!");
					f_close(&file);
					index = 0x00;
					reindex = 0x00;									
				}
				else
				{
					printf("\r\n��������ʧ��!");
					printf("\r\n�������: %u",res);									
				}
				break;
			}
			else if ((key < 0x21) || (key > 0x80))
			{
				continue;
			}
			else
			{
				file_buff[index++] = key;
				printf("%c",key);
				if (index > 512)
				{
					index = 0x00;
				}
			}
		}
	}
	else
	{
		printf("\r\n���ļ�ʧ��,�������: %u",res);
	}
}

void read_file(void)
{

	FATFS fs;
	FIL	file;
	FRESULT res; 
	DIR dirs;
	FILINFO finfo;
	char path[20]={0};
	char buffer[512] = {0};
	uint32_t i;
	uint8_t length;
	uint32_t re,files_num = 0;

	res = f_mount(&fs,"",0);

	if (res != FR_OK)
	{
		printf("\r\n�����ļ�ϵͳʧ��,�������: %u",res);
		return;
	}

	res = f_opendir(&dirs,"/");

	if (res == FR_OK) 
	{
		//i = strlen(path);
		printf("\r\n-----------�ļ��б�-------");
		while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) 
		{
			if (finfo.fattrib & AM_DIR)//������ļ��� 
			{ 
				continue;
			} 
			else 
			{   
				files_num++;
				//��ʾ�ļ���,��ʾ�ļ�ʵ�ʴ�С
				printf("\r\n/%12s%7ld Byte ",  &finfo.fname[0],finfo.fsize);
 			}
		}
		if( files_num == 0 )//���ļ�
		{
			printf("\r\n���ļ�,�뷵���ȴ����ļ�!");
			return;
		}
	}
	else
	{
		printf("\r\n�򿪸�Ŀ¼ʧ��!");
		printf("\r\n�������: %u",res);
	}
	printf("\r\n�����ļ�ȫ��,�Իس�����...");
	get_file_name((uint8_t *)path,length,FI);
	res = f_open(&file,path,FA_READ);

	if (res == FR_OK)
	{
		printf("\r\n���ڴ��ļ�,�������ļ�����:\r\n");
		while (1)
		{
			for(i = 0;i < 512;i++)
			{
				buffer[i] = 0x00;
			}
			res = f_read(&file,buffer,512,&re);
			printf("%s",buffer);

			if (res || re == 0)
			{
				printf("\r\n�ļ���ȡ����,�ر��ļ�!");
				f_close(&file);
				break;  
			}	
		}
	}
	else
	{
		printf("\r\n���ļ� %s ����\n",path);
	}
	f_mount(NULL,"",0);
}


void creat_dir(void)
{
	FATFS fs;        
	FRESULT res;     
	char path[20];
	uint8_t length;
	res = f_mount(&fs,"",0);
	if (res != FR_OK)
	{
		printf("\r\n�����ļ�ϵͳʧ��,�������: %u",res);
		return;
	}	
	printf("\r\n�������ļ�����,�س�ȷ��...��ʽ 8 + 3...");
	get_file_name((uint8_t *)path,length,DI);
	res = f_mkdir(path);
	if (res == FR_OK)
	{
		printf("\r\n�����ļ�Ŀ¼�ɹ�!");
	}
	else
	{
		printf("\r\n����Ŀ¼ʧ��...");
		printf("\r\n�������: %u",res);
	}
	f_mount(NULL,"",0);
}


void get_file_name(uint8_t *file_name,uint8_t length,uint8_t type)
{
	signed char key;
	uint8_t name_leng = 0;
	printf("\r\n");
	
	while (1)
	{
		if(xSerialGetChar( xPort, &key, comRX_BLOCK_TIME,serCOM2 )== pdFALSE)  
		{
			continue;
		}

		if ((key == 13) && (name_leng == 0))
		{
			printf("\r\n");
			continue;
		}
		else if ((key == 0x2F) || (key == 0x5C))
		{
			printf("%c",key);
			continue;
		}
		else if ((key == 13) && (name_leng > 0))
		{
			printf("\r\n");
			if (type == FI)
			{
				if (check_file_name(file_name,name_leng) == 0)
				{
					break;
				}
			}
			else
			{
				break;
			}			
			
		}
		else
		{
			printf("%c",key);
			file_name[name_leng] = key;
			name_leng++;
			if (name_leng > 12)
			{
				printf("\r\n�ļ�����ʽ: 8 + 3��ֻ֧��8���ַ�,3����չ��!");
				printf("\r\n����������...");
				name_leng = 0;
				continue;
			}
		}
	}
}
void format_disk(void)
{
	FATFS fs;
	uint8_t res;
	BYTE work[FF_MAX_SS]; /* Work area (larger is better for processing time) */
	
	res = f_mount(&fs,"",0);
	if (res != FR_OK)
	{
		printf("\r\n�����ļ�ϵͳʧ��,�������: %u",res);
		return;
	}	
	printf("\r\n���ڸ�ʽ������,���Ժ�...");

	res = f_mkfs("", FM_ANY,0,work,sizeof(work));
	if (res == FR_OK)
	{
		printf("\r\n��ʽ���ɹ�...");
	}
	else
	{
		printf("\r\n��ʽ��ʧ��...");
		printf("\r\n�������: %u\r\n",res);
	}
	f_mount(NULL,"",0);
}
void creat_file(void)
{
	FIL file;
	FIL *pf = &file;
	FATFS fs;
	uint8_t res;
	uint8_t name[16] = {0};
	uint8_t length = 0;
	printf("\r\n�������ļ���,�س�ȷ��...��ʽ 8 + 3...");
	printf("\r\n��:123.dat\r\n");
	get_file_name(name,length,FI);	
	res = f_mount(&fs,"",0);		                           /* Mount a Logical Drive 0 */
	if (res != FR_OK)
	{
		printf("\r\n�����ļ�ϵͳʧ��,�������: %u",res);
		return;
	}	
	res = f_open(pf,(TCHAR *)name,FA_READ | FA_WRITE | FA_CREATE_NEW);
	if (res == FR_OK)
	{
		printf("\r\n�����ļ��ɹ�!");
		res = f_close(pf);
		if (res != FR_OK)
		{
			printf("\r\n�����ļ��ɹ�,���ر��ļ�ʱ,ʧ��!");
			printf("\r\n�������: %u",res);				
		}				
	}
	else
	{
		printf("\r\n�����ļ�ʧ��!");
		printf("\r\n�������: %u",res);	
	}
	f_mount(NULL,"",0);
}
void delete_file(void)
{
	FATFS fs;
	FRESULT res;
	DIR dirs;
	FILINFO finfo;
	uint8_t name[16] = {0};
	uint8_t length = 0;
	uint32_t files_num = 0;

	res = f_mount(&fs,"",0);		      /* Mount a Logical Drive 0*/	 
	if (res != FR_OK)
	{
		printf("\r\n�����ļ�ϵͳʧ��,�������: %u",res);
		return;
	}

	res = f_opendir(&dirs,"/");		 /* Create a Directroy Object */
	if (res == FR_OK) 
	{
		printf("\r\n-----------�ļ��б�-------");
		while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) 
		{
			if (finfo.fattrib & AM_DIR)//������ļ��� 
			{ 
				continue;
			} 
			else 
			{   
				files_num++;
				//��ʾ�ļ���,��ʾ�ļ�ʵ�ʴ�С ,�ļ�ʵ�ʴ�С�����������뷨
				printf("\r\n/%12s%7ld KB ",  &finfo.fname[0],(finfo.fsize+512)/1024);
 			}
		}
		if( files_num == 0 )//���ļ�
		{
			printf("\r\n���ļ�,�뷵���ȴ����ļ�!");
			return;
		}
	}

	get_file_name(name,length,FI);    /* Get file name */
	
	res = f_unlink((TCHAR *)name);	  /* Delete a File or Directory */

	if (res == FR_OK)
	{
		printf("\r\nɾ���ļ��ɹ�!");
	}
	else if (res == FR_NO_FILE)
	{
		printf("\r\n�Ҳ����ļ���Ŀ¼!");
	}
	else if (res == FR_NO_PATH)
	{
		printf("\r\n�Ҳ���·��!");
	}
	else
	{
		printf("\r\n�������: %u",res);
	}
	f_mount(NULL,"",0);
}
void list_file(void)
{
	FATFS fs;
	FILINFO finfo;
	FRESULT res;
	DIR dirs;
	int i;
	int files_num=0;
	res = f_mount(&fs,"",0);
	if (res != FR_OK)
	{
		printf("\r\n�����ļ�ϵͳʧ��,�������: %u",res);
		return;
	}	
	res = f_opendir(&dirs,"/");							/* Create a Directroy Object */
	printf("\r\n------------�ļ��б�------------");
	if (res == FR_OK)
	{
		while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) 	 /* Read Directory Entry in Sequense*/
		{
			i = strlen(finfo.fname);
			if (finfo.fattrib & AM_DIR)//������ļ��� 
			{
				files_num++;
				printf("\r\n/%s", &finfo.fname[0]);
				switch(i)//���ã�����ļ��������
				{
				case 1:printf(" ");
				case 2:printf(" ");
				case 3:printf(" ");
				case 4:printf(" ");
				case 5:printf(" ");
				case 6:printf(" ");
				case 7:printf(" ");
				case 8:printf("%15s"," ");
				}	
			} 
			else 
			{
				continue;
			}	
		}
	}
	else
	{
		printf("\r\n�򿪸�Ŀ¼ʧ��!");
		printf("\r\n�������: %u",res);
	}
	res = f_opendir(&dirs, "/");
	if (res == FR_OK) 
	{
		while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) 
		{
			if (finfo.fattrib & AM_DIR) 
			{ 
				continue;
			} 
			else 
			{   
				files_num++;				
				printf("\r\n/.%12s%7ld KB ",  &finfo.fname[0],(finfo.fsize+512)/1024);				
			}
		}
		if( files_num==0 )//���ļ�
		{
			printf("\r\n���ļ�!");    
		}
	}
	else
	{
		printf("\r\n�򿪸�Ŀ¼ʧ��!");
		printf("\r\n�������: %u",res);
	}
	f_mount(NULL,"",0);

}
void get_disk_info(void)
{
	FATFS fs;
	FATFS *fls = &fs;
	FRESULT res;
	DWORD fre_clust;	
	
	res = f_mount(&fs,"",0);						 /* Mount a Logical Drive 0 */
	if (res != FR_OK)
	{
		printf("\r\n�����ļ�ϵͳʧ��,�������: %u",res);
		return;
	}	

	res = f_getfree("/",&fre_clust,&fls);		 /* Get Number of Free Clusters */
	if (res == FR_OK) 
	{
	                                             /* Print free space in unit of MB (assuming 4096 bytes/sector) */
        printf("\r\n%d KB Total Drive Space.\r\n"
               "%d KB Available Space.\r\n",
               ((fls->n_fatent-2)*fls->csize)*4,(fre_clust*fls->csize)*4);
	}
	else
	{
		printf("\r\n��ô�����Ϣʧ��!");
		printf("\r\n�������: %u",res);
	}																									   
	f_mount(NULL,"",0);						     /*Unmount a Logical Drive 0 */ 
}
uint8_t check_file_name(uint8_t *file_name,uint8_t length)
{
	uint8_t res;
	if (length > 13)
	{
		res = 1;
	}
	else
	{
		if (file_name[length - 4] == '.')
		{
			res = 0;
		}
		else
		{
			res = 2;
		}
	}
	return res;

}


void Sys_Soft_Reset(void)
{   
	SCB->AIRCR =0X05FA0000|(u32)0x04;	  
}



#include "minIni.h"
#define sizearray(a)  (sizeof(a) / sizeof((a)[0]))

const char inifile[] = "/ipcfg.ini";

static char MQTTServerIP[16]={0};
static int MQTTServerPort;
static char MQTTServerName[32]={0};
static char MQTTServerPassword[32]={0};

const char * getMQTTServerIP(void)
{
	return MQTTServerIP;
}

 
int getMQTTServerPort(void)
{
	 return  MQTTServerPort;
}

char * getMQTTServerName(void)
{
	return MQTTServerName;
}

char * getMQTTServerPassword(void)
{
	return MQTTServerPassword;
}

int InitMQTTServerInfo(void)
{
	FATFS fs;
	FRESULT res; 
	char str[32+1]={0};
	long n;
	//int s, k;
	//char section[50];
	res = f_mount(&fs,"",0);
	
	if (res != FR_OK)
	{
		printf("\r\nError load file system, code: %u",res);
		return -1;
	}
	
	
	n = ini_gets("server", "ip", "192.168.1.1", str, sizearray(str), inifile);	
	printf("\nserver ip:[ %s ]\n",str);
	memcpy(MQTTServerIP,str,n);
	
	n = ini_gets("server", "port", "1883", str, sizearray(str), inifile);	
	printf("server port:[ %s ]\n",str);
	MQTTServerPort = atoi(str);


	n = ini_gets("server", "name", "", str, sizearray(str), inifile);	
	printf("server name:[ %s ]\n",str);
	memcpy(MQTTServerName,str,n);
	//dprintf("%s\n",MQTTServerName);
	if(strcmp(MQTTServerName,"")==0)
	{
		dprintf("MQttServerName=\"\"\n");
	}

	n = ini_gets("server", "password", "", str, sizearray(str), inifile);	
	printf("server password:[ %s ]\n",str);
	memcpy(MQTTServerPassword,str,n);
	//dprintf("%s\n",MQTTServerPassword);
	if(strcmp(MQTTServerPassword,"")==0)
	{
		dprintf("MQTTServerPassword=\"\"\n");
	}	
		
	f_mount(NULL,"",0);
	return 0;
}

