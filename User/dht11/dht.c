#include "dht.h"


void DHT_TX()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIOA->BRR = GPIO_Pin_6;
	Delayus(30000);
	GPIOA->BSRR = GPIO_Pin_6;
	Delayus(20);

}

DHT11_Data_TypeDef DHT_RX()
{
	int data[40];
	//int data[40];
	int index;
	DHT11_Data_TypeDef rxData;
	float multiplier;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	//Pull-down signal for 80 us
	while((GPIOA->IDR&(1<<6)) == 0) { }
	
	//pull-up signal for 80 us
	while((GPIOA->IDR&(1<<6)) > 1) { }
	
	for (index = 0; index < 40; index++)
	{		
		while( (GPIOA->IDR&(1<<6)) == 0) { }
		Delayus(50);
		if ((GPIOA->IDR&(1<<6)) > 1)
		{
			data[index] = 1;
			while((GPIOA->IDR&(1<<6)) > 1) {}
		}
		else{
			data[index] = 0;
		}
	}
	
	//Data manipulation
	rxData.humi = 0;
	rxData.temp = 0;
	
	
	
	multiplier = 1;
	for (index = 7; index >= 0; index--)
	{
		rxData.humi += data[index]*multiplier;
		multiplier *= 2;
	}
	/*
	multiplier = 0.5;
	for (index = 8; index <= 15; index++)
	{
		rxData.humi += data[index]*multiplier;
		multiplier /= 2;
	}
	*/
	multiplier = 1;
	for (index = 23; index >= 16; index--)
	{
		rxData.temp += data[index]*multiplier;
		multiplier *= 2;
	}
	/*
	multiplier = 0.5;
	for (index = 24; index < 32; index++)
	{
		rxData.temp += data[index]*multiplier;
		multiplier /= 2;
	}
	*/
	return rxData;
}
