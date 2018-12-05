/**
  ******************************************************************************
  * @file    bsp_hc05.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   HC05����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:���� ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "bsp_hc05.h"
#include "bsp_usart_blt.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//�����豸�б�����main�ļ��ж���
extern  BLTDev bltDevList;
extern unsigned int Task_Delay[]; 

 /**
  * @brief  ��ʼ������LED��IO
  * @param  ��
  * @retval ��
  */
static void HC05_GPIO_Config(void)
{		
		GPIO_InitTypeDef GPIO_InitStructure;

		/*����GPIOʱ��*/
		RCC_APB2PeriphClockCmd( BLT_INT_GPIO_CLK|BLT_KEY_GPIO_CLK, ENABLE); 

		GPIO_InitStructure.GPIO_Pin = BLT_INT_GPIO_PIN;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(BLT_INT_GPIO_PORT, &GPIO_InitStructure);	
	
		
		GPIO_InitStructure.GPIO_Pin = BLT_KEY_GPIO_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(BLT_KEY_GPIO_PORT, &GPIO_InitStructure);	

}



						   
	
 /**
  * @brief  ��HC05ģ�鷢��������OK��ֻ�����ھ���OKӦ�������
	* @param  cmd:����������ַ�������Ҫ��\r\n��
	* @param	clean ����������Ƿ�������ջ�������1 �����0 �����
	* @template  ��λ���	HC05_Send_CMD("AT+RESET\r\n",1);	
  * @retval 0,���óɹ�;����,����ʧ��.	
  */
uint8_t HC05_Send_CMD(char* cmd,uint8_t clean)
{	 		 
	uint8_t retry=5;
	uint8_t i,result=1;
	
	while(retry--)
	{
		BLT_KEY_HIGHT;
		delay_ms(10);
		Usart_SendString(HC05_USART,(uint8_t *)cmd);
		
			for(i=0;i<20;i++)
			{ 
				uint16_t len;
				char * redata;
				
				delay_ms(10);
				
				redata = get_rebuff(&len); 
				if(len>0)
				{
					if(redata[0]!=0)
					{
						HC05_DEBUG("send CMD: %s",cmd);

						HC05_DEBUG("receive %s",redata);
					}
					if(strstr(redata,"OK"))				
					{
						
						if(clean==1)
							clean_rebuff();
						return 0;
					}
					else
					{
						//clean_rebuff();
					}
				}
				else
				{					
					delay_ms(100);
				}		
			}
			HC05_DEBUG("HC05 send CMD fail %d times",retry);
		}
	
	HC05_DEBUG("HC05 send CMD fail ");
		
	if(clean==1)
		clean_rebuff();

	return result ;

}

 /**
  * @brief  ʹ��HC05͸���ַ�������
	* @param  str,Ҫ������ַ���
  * @retval ��
  */
void HC05_SendString(char* str)
{
		BLT_KEY_LOW;
	
		Usart_SendString(HC05_USART,(uint8_t *)str);

}



 /**
  * @brief  ��ʼ��GPIO�����HC05ģ��
  * @param  ��
  * @retval HC05״̬��0 ��������0�쳣
  */
uint8_t HC05_Init(void)
{
	uint8_t i;
	
	HC05_GPIO_Config();

	BLT_USART_Config();
	
	for(i=0;i<BLTDEV_MAX_NUM;i++)
	{
		sprintf(bltDevList.unpraseAddr[i]," ");
		sprintf(bltDevList.name[i]," ");

	}	
		bltDevList.num = 0;

	return HC05_Send_CMD("AT\r\n",1);
}





 /**
  * @brief  �ѽ��յ����ַ���ת����16������ʽ�����ֱ���(��Ҫ����ת��������ַ)
	* @param  ����������ַ���
  * @retval ת��������ֱ���	
  */
unsigned long htoul(const char *str)
{


  long result = 0;

  if (!str)
    return 0;

  while (*str)
  {
    uint8_t value;

    if (*str >= 'a' && *str <= 'f')
      value = (*str - 'a') + 10;
    else if (*str >= 'A' && *str <= 'F')
      value = (*str - 'A') + 10;
    else if (*str >= '0' && *str <= '9')
      value = *str - '0';
    else
      break;

    result = (result * 16) + value;
    ++str;
  }

  return result;
}


 /**
  * @brief  ��str�У�������ǰ���prefix�ַ���,
						��strΪ"abcdefg",prefixΪ"abc"������ñ������󷵻�ָ��"defg"��ָ��
	* @param  str,ԭ�ַ���
	* @param  str_length���ַ�������
	* @param 	prefix��Ҫ�������ַ���
  * @retval ����prefix����ַ���ָ��
  */

char *skipPrefix(char *str, size_t str_length, const char *prefix)
{

  uint16_t prefix_length = strlen(prefix);

  if (!str || str_length == 0 || !prefix)
    return 0;

  if (str_length >= prefix_length && strncmp(str, prefix, prefix_length) == 0)
    return str + prefix_length;

  return 0;
}

 /**
  * @brief  ��stream�л�ȡһ���ַ�����line��
	* @param  line,�洢����е��ַ�������
	* @param  stream��ԭ�ַ���������
	* @param 	max_size��stream�Ĵ�С 
  * @retval line�ĳ��ȣ���stream��û�С�\0����'\r'��'\n'���򷵻�0
  */
int get_line(char* line, char* stream ,int max_size)  
{  
		char *p;	
    int len = 0;  
		p=stream;
    while( *p != '\0' && len < max_size ){  
        line[len++] = *p;  
				p++;
        if('\n' == *p || '\r'==*p)  
            break;  
    }
	
		if(*p != '\0' && *p != '\n' && *p != '\r')
			return 0;
  
      
    line[len] = '\0';  
    return len;  
} 


 /**
  * @brief  ��HC05д����������ģ�����Ӧ
	* @param  command ��Ҫ���͵�����
	* @param  arg�����������Ϊ0ʱ������������commandҲΪ0ʱ������"AT"����
  * @retval ��
  */
void writeCommand(const char *command, const char *arg)
{
		char str_buf[50];

		BLT_KEY_HIGHT;
		delay_ms(10);

		if (arg && arg[0] != 0)
			sprintf(str_buf,"AT+%s%s\r\n",command,arg);
		else if (command && command[0] != 0)
		{
			sprintf(str_buf,"AT+%s\r\n",command);
		}
		else
			sprintf(str_buf,"AT\r\n");

		HC05_DEBUG("CMD send:%s",str_buf);
		
		Usart_SendString(HC05_USART,(uint8_t *)str_buf);

}


 

 /**
  * @brief  ɨ���ܱߵ������豸�����洢���豸�б��С�
	* @param  bltDev �������豸�б�ָ��
  * @retval �Ƿ�ɨ�赽�豸��0��ʾɨ�赽����0��ʾû��ɨ�赽
  */
uint8_t parseBluetoothAddress(BLTDev *bltDev)
{
  /* Address should look like "+ADDR:<NAP>:<UAP>:<LAP>",
   * where actual address will look like "1234:56:abcdef".
   */

	char* redata;
	uint16_t len;
	
	char linebuff[50];
	uint16_t linelen;
	
	uint16_t getlen=0;
	uint8_t linenum=0;	
	
	
	uint8_t i;
	
	char *p;
	char *r;
	int rssi;

	HC05_Send_CMD("AT+INQ\r\n",0);

	redata =get_rebuff(&len);
	
		
	if(redata[0] != 0 && strstr(redata, "+INQ:") != 0)
	{
		HC05_DEBUG("rebuf =%s",redata);

getNewLine:
		while(getlen < len-2*linenum )
		{	
			linelen = get_line(linebuff,redata+getlen+2*linenum,len);
			if(linelen>50 && linelen != 0)
			{
				HC05_Send_CMD("AT+INQC\r\n",1);//�˳�ǰ�жϲ�ѯ
				return 1;
			}
			
			getlen += linelen;
			linenum++;
					
			p = skipPrefix(linebuff,linelen,"+INQ:");
			if(p!=0)
			{				
				uint8_t num ;
				num = bltDev->num;
				
				strBLTAddr(bltDev,':');
				
				for(i=0;i<=num;i++)
				{
					if(strstr(linebuff,bltDev->unpraseAddr[i]) != NULL)	
					{
						//HC05_INFO("Rawdata p: %s", p);
						r = strchr(p,',');
						r = strchr(++r,',');
						if (r == 0)
						{
							HC05_Send_CMD("AT+INQC\r\n",1);//�˳�ǰ�жϲ�ѯ
							return 1;
						}
						//HC05_INFO("Rawdata r: %s", ++r);
						
						//Now r is the hex 2s complement string of RSSI
						rssi = 0-(int)65536+(int)htoul(++r);
						//HC05_INFO("Rawdata r: %d", rssi);
						bltDev->RSSI[i] = rssi;
						
						goto getNewLine;	//!=nullʱ����ʾ�õ�ַ��������ĵ�ַ��ͬ
					}
				}							
					
				/*�������豸�����б��У��Ե�ַ���н���*/	
				bltDev->addr[num].NAP = htoul(p);
				
				//HC05_INFO("Rawdata p: %s", p);
				r = strchr(p,',');
				r = strchr(++r,',');
				if (r == 0)
				{
					HC05_Send_CMD("AT+INQC\r\n",1);//�˳�ǰ�жϲ�ѯ
					return 1;
				}
				//HC05_INFO("Rawdata r: %s", ++r);
				
				//Now r is the hex 2s complement string of RSSI
				rssi = 0-(int)65536+(int)htoul(++r);
				//HC05_INFO("Rawdata r: %d", rssi);
				bltDev->RSSI[num] = rssi;
				
				p = strchr(p,':');				
				
				if (p == 0)
				{
					HC05_Send_CMD("AT+INQC\r\n",1);//�˳�ǰ�жϲ�ѯ
					return 1;
				}

				bltDev->addr[num].UAP = htoul(++p);
				p = strchr(p,':');

				if (p == 0)
				{
					HC05_Send_CMD("AT+INQC\r\n",1);//�˳�ǰ�жϲ�ѯ
					return 1;
				}

				bltDev->addr[num].LAP = htoul(++p);
				
				/*�洢������ַ(�ַ�����ʽ)*/
				sprintf(bltDev->unpraseAddr[num],"%X:%X:%X",bltDev->addr[num].NAP,bltDev->addr[num].UAP,bltDev->addr[num].LAP);

				bltDev->num++;
				
			}


		}
		
		clean_rebuff();
		
		HC05_Send_CMD("AT+INQC\r\n",1);//�˳�ǰ�жϲ�ѯ
		return 0;
	}
	
	else
	{
		clean_rebuff();
		
		HC05_Send_CMD("AT+INQC\r\n",1);//�˳�ǰ�жϲ�ѯ
		return 1;	
	}

}

 /**
  * @brief  ��������ַת�����ַ�����ʽ
	* @param  bltDev �������豸�б�ָ��
	*	@param  delimiter, �ָ����� ������Ҫʹ��':'��','��
  * @retval ��
  */
void strBLTAddr(BLTDev *bltDev,char delimiter)  
{
	uint8_t i;
	

	if(bltDev->num==0)
	{
		HC05_DEBUG("/*******No other BLT Device********/");
	}
	else
	{
		for(i=0;i<bltDev->num;i++)
		{
			sprintf(bltDev->unpraseAddr[i],"%X%c%X%c%X",bltDev->addr[i].NAP,delimiter,bltDev->addr[i].UAP,delimiter,bltDev->addr[i].LAP);
		}
	}

}


 /**
  * @brief  ��ȡԶ�������豸������
	* @param  bltDev �������豸�б�ָ��
  * @retval 0��ȡ�ɹ�����0���ɹ�
  */
uint8_t getRemoteDeviceName(BLTDev *bltDev)
{
	uint8_t i;
	char *redata;
	uint16_t len;
	char linebuff[50];
	uint16_t linelen;
	char *p;
	
	char cmdbuff[100];
	
	strBLTAddr(bltDev,',');

	HC05_DEBUG("device num =%d",bltDev->num);
	
	for(i=0;i<bltDev->num;i++)
	{
		sprintf(cmdbuff,"AT+RNAME?%s\r\n",bltDev->unpraseAddr[i]);
		HC05_Send_CMD(cmdbuff,0);
		
		redata =get_rebuff(&len);
		if(redata[0] != 0 && strstr(redata, "OK") != 0)
		{
			
			linelen = get_line(linebuff,redata,len);
			if(linelen>50 && linelen !=0 ) linebuff[linelen]='\0';	//�����ض�
					
			p = skipPrefix(linebuff,linelen,"+RNAME:");
			if(p!=0)
			{
				strcpy(bltDev->name[i],p);
			}

		}
		else
		{
			clean_rebuff();
			return 1;	
		}
		
		clean_rebuff();
	
	}
	
	return 0;

}

 /**
  * @brief  ��������豸�б�
	* @param  bltDev �������豸�б�ָ��
  * @retval ��
  */


void printBLTInfo(BLTDev *bltDev)  
{
	uint8_t i;

	if(bltDev->num==0)
	{
		HC05_DEBUG("/*******No remote BLT Device or in SLAVE mode********/");
	}
	else
	{
		HC05_DEBUG("ɨ�赽 %d �������豸",bltDev->num);

		for(i=0;i<bltDev->num;i++)
		{
			HC05_INFO("/*******Device[%d]********/",i);	
			HC05_INFO("Device Addr: %s",bltDev->unpraseAddr[i]);
			HC05_INFO("Device name: %s",bltDev->name[i]);
			HC05_INFO("Device rssi: %d",bltDev->RSSI[i]);
		}
	}

}



 /**
  * @brief  ɨ�������豸�������������к���"HC05"���豸
	* @param  ��
  * @retval 0��ȡ�ɹ�����0���ɹ�
  */
uint8_t linkHC05(void)
{
	//uint8_t i=0;
	//char cmdbuff[100];
	
	parseBluetoothAddress(&bltDevList);
	//getRemoteDeviceName(&bltDevList);
	//printBLTInfo(&bltDevList);
	
	
//	for(i=0;i<=bltDevList.num;i++)
//	{
//		if(strstr(bltDevList.name[i],"HC05") != NULL) //��NULL��ʾ�ҵ������Ʋ���ΪHC05���豸
//		{
//			HC05_INFO("������Զ��HC05ģ�飬���������������...");
//			
//			strBLTAddr(&bltDevList,',');		
//			
//			//���
//			sprintf(cmdbuff,"AT+PAIR=%s,20\r\n",bltDevList.unpraseAddr[i]);
//			HC05_Send_CMD(cmdbuff,0);
//				
//			//����	
//			sprintf(cmdbuff,"AT+LINK=%s\r\n",bltDevList.unpraseAddr[i]);
//						
//			return HC05_Send_CMD(cmdbuff,0);		
//		}
//	
//	}
	
	return 1;

}

void bleInit()
{
	char hc05_mode[10]="SLAVE";
	char hc05_name[30]="HC05_SLAVE";
	char hc05_nameCMD[40];
	
	
	
	//HC05_INFO("**********HC05ģ��ʵ��************");
	if(HC05_Init() != 0)
	{
		//HC05_ERROR("HC05ģ���ⲻ����������ģ���뿪��������ӣ�Ȼ��λ���������²��ԡ�");
		while(1);
	}
	
	/*��λ���ָ�Ĭ��״̬*/
	HC05_Send_CMD("AT+RESET\r\n",1);	
	delay_ms(800);
	
	HC05_Send_CMD("AT+ORGL\r\n",1);
	delay_ms(200);

	//Commands demo
	HC05_Send_CMD("AT+VERSION?\r\n",1);
	
	HC05_Send_CMD("AT+ADDR?\r\n",1);
	
	HC05_Send_CMD("AT+UART?\r\n",1);
	
	HC05_Send_CMD("AT+CMODE?\r\n",1);
	
	HC05_Send_CMD("AT+STATE?\r\n",1);	

	HC05_Send_CMD("AT+ROLE=0\r\n",1);
	
	/*��ʼ��SPP�淶*/
	HC05_Send_CMD("AT+INIT\r\n",1);
	HC05_Send_CMD("AT+CLASS=0\r\n",1);
	HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);
	
	/*����ģ������*/
	sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
	HC05_Send_CMD(hc05_nameCMD,1);

	//HC05_INFO("��ģ������Ϊ:%s ,ģ����׼��������",hc05_name);
	
	//Master
	
	HC05_Send_CMD("AT+RESET\r\n",1);
	delay_ms(800);
	HC05_Send_CMD("AT+ROLE=1\r\n",1);
	delay_ms(100);
	
	sprintf(hc05_mode,"MASTER");
	//HC05_INFO("HC05 mode  = %s",hc05_mode);
	
	sprintf(hc05_name,"HC05_%s_%d",hc05_mode,(uint8_t)rand());
	sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);	
	
	HC05_Send_CMD(hc05_nameCMD,1);

		
	HC05_Send_CMD("AT+INIT\r\n",1);
	HC05_Send_CMD("AT+CLASS=0\r\n",1);
	HC05_Send_CMD("AT+INQM=1,5,3\r\n",1);	
	
	//��������豸�б�
	bltDevList.num = 0;
}

void rssiScan()
{
	bltDevList.num = 0;
	//HC05_INFO("Scanning with RSSI:");
	linkHC05();

}