#ifndef __PHORES_H
#define __PHORES_H


#include "stm32f10x.h"

#define phores_threshold 0x600

void ADC_INIT(void);
void BUZZER_INIT(void);
void BUZZER_DEINIT(int cancel);
int check(int is_open);

#endif /* __PHORES_H */