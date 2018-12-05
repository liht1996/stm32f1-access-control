#include "phores.h"
#include "./led/bsp_led.h"


ADC_InitTypeDef ADC_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;

void Delay__us(int duration);

void ADC_INIT()
{
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//Clock
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	//Channel
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_55Cycles5);
	//Enable
	ADC_Cmd(ADC1, ENABLE);
	//Self-calibration
	
	/* Enable ADC1 reset calibration register */
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));
	/* Start ADC1 calibration */
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));
}

void BUZZER_INIT()
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	RCC->CFGR |= (RCC_MCO_SYSCLK << 24);
	
}

void BUZZER_DEINIT(int cancel){
	if (cancel == 1){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
}
	

int check(int is_open){
	int res_value;
	int i;
	if (is_open == 1){
		//BUZZER_DEINIT(1);
		return 0;
	}
	else{
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		res_value = ADC_GetConversionValue(ADC1);
//		Delay__us(10);
//		res_value[1] = ADC_GetConversionValue(ADC1);
//		Delay__us(10);
//		res_value[2] = ADC_GetConversionValue(ADC1);

		
		if (res_value < phores_threshold){
			//BUZZER_INIT();
			return 1;
		}
		else{
			;
			//BUZZER_DEINIT(1);
		}
		ADC_SoftwareStartConvCmd(ADC1, DISABLE);
		return 0;
	}
}