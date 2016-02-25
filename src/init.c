#include "init.h"
void DMA_init(void)
{
	RCC->AHBENR|=RCC_AHBENR_DMA1EN;								// включаем такты
	DMA1_Channel1->CPAR = (uint32_t) &ADC1->DR;		// адрес откуда DMAшить
	DMA1_Channel1->CMAR = (uint32_t) &ADC_DATA;		// адрес куда DMAшить
	
	
	
}

void TIM_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; // вкл такты TIM1
	TIM1->PSC = 0;
	TIM1->ARR = 1633 ; 									//Fs = 44090.6307
	TIM1->CR2|= TIM_CR2_MMS_1; 					//010: Update - The update event is selected as trigger output (TRGO)
	TIM1->CR1|= TIM_CR1_CEN;						//Запуск счета
}

void GPIO_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // вкл такты GPIOC 
	GPIOC->CRL &= ~GPIO_CRL_CNF;
	GPIOC->CRL &= ~GPIO_CRL_MODE0;
}

void ADC_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;					 					// вкл такты ADC1
	ADC1->JSQR &= ~ADC_JSQR_JL;														// JL = 1 преобразование
	ADC1->JSQR |= (10<<15);																// JSQ4 = 10 канал
	ADC1->CR2 |= ADC_CR2_ADON; 														// вкл питание
	ADC1->CR2 &= ~ADC_CR2_JEXTSEL;												// EXTSEL=000: Timer 1 TRGO event
	ADC1->CR2 |= ADC_CR2_JEXTTRIG;  						  				// включаем запуск от внешнего события
	ADC1->SMPR1 |= ADC_SMPR1_SMP10_2 | ADC_SMPR1_SMP10_1; // 11
	ADC1->SMPR1 &= ~ADC_SMPR1_SMP10_0; 										// 110
	ADC1->CR2 |= ADC_CR2_DMA;															// включаю DMA
	
}