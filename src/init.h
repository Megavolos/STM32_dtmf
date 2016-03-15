#ifndef INIT_H
#define INIT_H

#include "stm32f10x.h"
#include "sdft.h"

void DMA_init(void);
void GPIO_init(void);
void ADC_init(void);
void TIM_init(void);

#endif
