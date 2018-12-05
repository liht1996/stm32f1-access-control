#include "stm32f10x.h"
#include "lcd.h"
#include <cstring>
#include "stdio.h"
#include "dht.h"
#include "utility.h"
#include "keyboard.h"
#include "bsp_SysTick.h"
#include "bsp_usart_blt.h"
#include "bsp_usart.h"
#include "bsp_hc05.h"
#include "pwm.h"
#include "phores.h"
#include "./ov7725/bsp_ov7725.h"
#include "./led/bsp_led.h"
#include "./bmp/bsp_bmp.h"
#include "ff.h"

#include "Common.h"
#include "bsp_esp8266.h"
/*------------------------------------------------------------
PROJ Template
-------------------------------------------------------------*/

//BLE Defination
unsigned int Task_Delay[NumOfTask]; 
BLTDev bltDevList;

//Camera Defination
extern uint8_t Ov7725_vsync;

extern OV7725_MODE_PARAM cam_mode;

int LCD_X_LENGTH = 320;
int LCD_Y_LENGTH = 240;
FATFS fs;
FRESULT res_sd;

//Wifi defination
uint8_t ucId, ucLen;
uint8_t ucLed1Status = 0, ucLed2Status = 0, ucLed3Status = 0, ucBuzzerStatus = 0;

char cStr [ 100 ] = { 0 }, cCh;

char * pCh, * pCh1;


//char temp[10]="0000000000";



DHT11_Data_TypeDef dhtData;
char keys[5];
int it = 0;
enum keyIn lastKey = FREE;
enum keyIn nowKey;
char password[] = "6789";
int open = 0;
char dhtTemp[50];
char dhtHumi[50];
char name[40];
int numOfPic = 0;
int name_count = 0;
int delay = 0;





void initAll()
{
	SysTick_Init();
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	LCD_INIT();
	MCO_INIT();
	LED_GPIO_Config();
	LCD_DrawString(20, 50, "Initializing");
	//USART_Config();			//Printing
	ESP8266_Init ();
	ADC_INIT();
	bleInit();
	keyboardConfiguration();
	//LCD_DrawString(20, 50, "Initialization Completed");
	//Delayus(1000000);
	LCD_Clear ( 0, 0, 240, 320, BACKGROUND);
}

int main(void)
{
	

	int j = 10;	//iteration for bluetooth
	int k = 100; // iteration for Wifi
	initAll();
	
	//Main loop
	while (1) {
		int i;		//iteration for password
		delay --;
		if(open == 1)
		{
			
			LCD_DrawString(50, 100, "Open");
			/*
			DHT_TX();
			dhtData = DHT_RX();
			sprintf(dhtHumi, "Humidity:    %d", dhtData.humi);
			sprintf(dhtTemp, "Temperature: %d", dhtData.temp);
			LCD_DrawString(0,250,dhtHumi);
			LCD_DrawString(0,270,dhtTemp);
			*/
			Delayus(2000000);
			LCD_Clear ( 0, 90, 240, 320, BACKGROUND);
			
			//ESP8266_Rst();
			//Camera
			res_sd = f_mount(&fs,"0:",1);
      Ov7725_vsync = 0;
			
			OV7725_GPIO_Config();
			//Delayus(10000000);
			OV7725_Window_Set(cam_mode.cam_sx,
										cam_mode.cam_sy,
										cam_mode.cam_width,
										cam_mode.cam_height,
										cam_mode.QVGA_VGA);
			LCD_GramScan( cam_mode.lcd_scan );
			name_count++;
			for (numOfPic = 0; numOfPic<5; numOfPic++){
				sprintf(name,"0:photo_%d_%d.bmp",name_count,numOfPic);
				Delayus(100000);
					if( Ov7725_vsync == 2 ){
						FIFO_PREPARE;             
						ImagDisp(cam_mode.lcd_sx,
							 cam_mode.lcd_sy,
							 cam_mode.cam_width,
							 cam_mode.cam_height, numOfPic);            
						LCD_GramScan ( cam_mode.lcd_scan );
						
						if(numOfPic != 0){
							if(Screen_Shot(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH, name)==0){
								//LED_GREEN;
								Delayus(500000);
							}
							//LED_RGBOFF;
							Ov7725_vsync = 0;
							}
						
						}
			}
			LCD_GramScan(1);
			OV7725_DEINIT();
			MCO_INIT();
			openDoor();
			LCD_Clear(0,0,240,320,BACKGROUND);
			LCD_DrawString(20,100, "GoodBye!!!");
			//last part
			open = 0;
			//printf("\nRe-init\n");
			Delayus(3000000);
			//ESP8266_Init ();
			//bleInit();
			keyboardConfiguration();
			MCO_INIT();
			LCD_Clear(0,0,240,320,BACKGROUND);
			delay = 500;
		}
		
		//Reset display
		if (it == 0)
		{
			for (i = 0; i < 4; i++){keys[i] = '*';}
		}
		//Read Keyboard Input & Compare
		nowKey = getKeyboardInput();
		if (lastKey==FREE && nowKey!=FREE)
		{
			char temp_char = decodeKey(nowKey);
			if (temp_char != 'X'){
				if (temp_char == 'D')
				{
					for (i = 0; i < 4; i++){
						keys[i] = '*';
						it = 0;
					}
				}
				else if(temp_char == 'A')
				{
					if (it!= 0){
						it--;
						keys[it] = '*';
					}
				}
				else{
					keys[it] = temp_char;
					it++;
					if (it == 4)
					{
						if (strcmp(keys, password) == 0){
							open = 1;
							Delayus(1000000);
						}
						else{
							LCD_DrawString(50, 100, "Invalid");
							Delayus(1000000);
						}
						it = 0;						
					}
				}
			}
		}
		lastKey = nowKey;

		LCD_DrawString(80,10,keys);
		//Delayus(50000);
		
		
		
		
		
		
		
		
		//Wi-fi part

			if ( strEsp8266_Fram_Record .InfBit .FramFinishFlag )
			{
				USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, DISABLE ); //禁用串口接收中断
				strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';
				if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CMD_LED_" ) ) != 0 ) 
				{
					cCh = * ( pCh + 8 );
					if (cCh == '2')
					{
						printf("\nClick!\n");
						open = (open+1)%2;
					}
					sprintf ( cStr, "CMD_LED_%d_%d_%d_ENDLED_END", ucLed1Status, ucLed2Status, ucLed3Status );
				}
				/*
				else if ( ( ( pCh  = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CMD_UART_" ) ) != 0 ) && 
									( ( pCh1 = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "_ENDUART_END" ) )  != 0 ) ) 
				{
					if ( pCh < pCh1)
					{
						ucLen = pCh1 - pCh + 12;
						memcpy ( cStr, pCh, ucLen );
						cStr [ ucLen ] = '\0';
					}
				}
				
				if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+IPD," ) ) != 0 ) 
				{
					ucId = * ( pCh + strlen ( "+IPD," ) ) - '0';
					ESP8266_SendString ( DISABLE, cStr, strlen ( cStr ), ( ENUM_ID_NO_TypeDef ) ucId );
				}
				*/
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;	
				USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, ENABLE ); //使能串口接收中断
			}

		//BLE part
		if (j < 0)
		{
			//bleInit();
			//HC05_INFO("Scan start");	
			LCD_Clear(40,90,240,120,BACKGROUND);
			LCD_DrawString(80, 40, "Scanning");
			rssiScan();
			//HC05_INFO("Scan end");	
			for(j = 0; j < bltDevList.num; j++)
			{
				if((strcmp(bltDevList.unpraseAddr[j], "7C6B:9C:A33C86") == 0) && (bltDevList.RSSI[j] > -60))
				{
					//Access RSSI in list
					open = 1;
				}
			}
			LCD_Clear(70, 30, 90, 50, BACKGROUND);
			j = 1000;
		}
		else{
			//LCD_Clear(0,0,20,20,BACKGROUND);
			j--;
			//if(j%100==0)	LCD_DrawString(0,0,"A");
		}
		if (delay<=0){
		if (check(open))
			LCD_DrawString(50, 100, "Force entry dectected!");
		else
			LCD_Clear(40,90,240,120,BACKGROUND);
		}

	}
}



// Name, J value 
// Buzzer or LED
// J has to be 10000. Otherwise keyboard cannot be used.
