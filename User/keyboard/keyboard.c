#include "keyboard.h"


void keyboardConfiguration()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

enum keyIn getKeyboardInput()
{
	int i;
	//Scan Row#1 D2
	GPIOD->BRR = GPIO_Pin_2;
	for(i = 8; i <12; i++)
	{
		if ((~GPIOC->IDR) & (1<<i))
		{
			//char info[50];
			//sprintf(info, "Row 1 Column %d", i-7);
			//sprintf(info, "%d", GPIOC->IDR );
			//LCD_DrawString(50, 15*i, info);
			return i-8;
		}
	}
	
	GPIOD->BSRR = GPIO_Pin_2;
	
	//C12
	GPIOC->BRR = GPIO_Pin_12;
	for(i = 8; i <12; i++)
	{
		if (~GPIOC->IDR&(1<<i))
		{
			//char info[50];
			//sprintf(info, "Row 2 Column %d", i-7);
			//LCD_DrawString(50, 15*i, info);
			return i+4-8;
		}
	}
	
	GPIOC->BSRR = GPIO_Pin_12;
	//C6  --  A5
	GPIOA->BRR = GPIO_Pin_5;
	for(i = 8; i <12; i++)
	{
		if (~GPIOC->IDR&(1<<i))
		{
			//char info[50];
			//sprintf(info, "Row 3 Column %d", i-7);
			//LCD_DrawString(50, 15*i, info);
			return i+8-8;
		}
	}
	GPIOA->BSRR = GPIO_Pin_5;
	//C7  --  A7
	GPIOA->BRR = GPIO_Pin_7;
	for(i = 8; i <12; i++)
	{
		if (~GPIOC->IDR&(1<<i))
		{
			//char info[50];
			//sprintf(info, "Row 4 Column %d", i-7);
			//LCD_DrawString(50, 15*i, info);
			return i+12-8;
		}
	}
	GPIOA->BSRR = GPIO_Pin_7;
	
	return FREE;
}



char decodeKey(enum keyIn in)
{
	LCD_Clear(0, 50 , 240, 320, BACKGROUND);
	if(in == K1){ return '1';}
	if(in == K2){ return '2';}
	if(in == K3){ return '3';}
	if(in == K4){ return '4';}
	if(in == K5){ return '5';}
	if(in == K6){ return '6';}
	if(in == K7){ return '7';}
	if(in == K8){ return '8';}
	if(in == K9){ return '9';}
	if(in == K0){ return '0';}
	if(in == KD){ return 'D';}
	if(in == KA){ return 'A';}
	LCD_DrawString(50, 50, "INPUT IS WRONG");
	LCD_DrawString(50, 70, "PLEASE RE-ENTER");
	
	return 'X';
}

