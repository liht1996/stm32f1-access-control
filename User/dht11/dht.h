#include "stm32f10x.h"
#include "utility.h"

typedef struct
{
	uint16_t  humi;
	uint16_t  temp;
}DHT11_Data_TypeDef;


void	DHT_TX(void);
DHT11_Data_TypeDef	DHT_RX(void);
