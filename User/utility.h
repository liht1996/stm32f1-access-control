#include "stm32f10x.h"
//#include "stdio.h"


void Delayus(int duration);

#define Delayms(x) Delayus(1000*x)

void OV7725_DEINIT(void);
