#include "init.h"
#define BUFFER_SIZE 147
//extern SDFT_VARS sdft_vars;
uint16_t ADC_DATA[BUFFER_SIZE];
void DMA_init(void)
{
	RCC->AHBENR|=RCC_AHBENR_DMA1EN;								// включаем такты
	DMA1_Channel1->CPAR = (uint32_t) &ADC1->DR;		// memory address where stored ADC result
	DMA1_Channel1->CMAR = (uint32_t) ADC_DATA;		// place where ADC samples put. 
	DMA1_Channel1->CNDTR = BUFFER_SIZE; 				  //length of ADC_DATA[] array
	DMA1_Channel1->CCR |=  DMA_CCR1_TCIE; //прерывание по половине буфера и по окончанию
	DMA1_Channel1->CCR &= ~DMA_CCR1_DIR; 					//Указываем направление передачи данных, из периферии в память
	DMA1_Channel1->CCR &= ~DMA_CCR1_PINC; 				//Адрес периферии не инкрементируем после каждой пересылки
	DMA1_Channel1->CCR |= DMA_CCR1_MINC; 					//Адрес памяти инкрементируем после каждой пересылки.
	DMA1_Channel1->CCR |= DMA_CCR1_PSIZE_0; 			//Размерность данных периферии - 16 бит
	DMA1_Channel1->CCR |= DMA_CCR1_MSIZE_0; 			//Размерность данных памяти - 16 бит
	DMA1_Channel1->CCR |= DMA_CCR1_PL; 						//Приоритет - очень высокий 
	DMA1_Channel1->CCR |= DMA_CCR1_CIRC; 					//Разрешаем работу DMA в циклическом режиме
	DMA1_Channel1->CCR |= DMA_CCR1_EN; 						//Разрешаем работу 1-го канала DMA
	NVIC_SetPriority(DMA1_Channel1_IRQn,2);
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	DMA1->IFCR|= DMA_IFCR_CGIF1;
	

	
}

void TIM_init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // вкл такты TIM1
	TIM3->PSC = 0;
	TIM3->ARR = 1633 ; 									//Fs = 44090.6307
	TIM3->CR2|= TIM_CR2_MMS_1; 					//010: Update - The update event is selected as trigger output (TRGO)
	TIM3->CR1|= TIM_CR1_CEN;						//Запуск счета
}

void GPIO_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // вкл такты GPIOC 
	RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
	GPIOC->CRL &= ~GPIO_CRL_CNF;
	GPIOC->CRL &= ~GPIO_CRL_MODE0;
	GPIOC->CRL |= GPIO_CRL_MODE6_1;
	GPIOC->CRL &= ~GPIO_CRL_CNF6;
	GPIOC->CRL |= GPIO_CRL_MODE7_1;
	GPIOC->CRL &= ~GPIO_CRL_CNF7;
	
	GPIOD->CRH &= ~GPIO_CRH_CNF;
	GPIOD->CRH &= ~GPIO_CRL_CNF;
	GPIOD->CRH |= GPIO_CRH_MODE13_1;
	GPIOD->CRH &= ~GPIO_CRH_CNF13;
	GPIOD->CRL |= GPIO_CRL_MODE6_1;
	GPIOD->CRL &= ~GPIO_CRL_CNF6;
	
}

void ADC_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;					 					// вкл такты ADC1
	ADC1->SQR1 &= ~ADC_SQR1_L;														// JL = 1 преобразование
	ADC1->SQR3 |= ADC_SQR3_SQ1_1|ADC_SQR3_SQ1_3;					// SQR3 SQ1 = 10 канал
	ADC1->CR2 |= ADC_CR2_ADON; 														// вкл питание
	ADC1->CR2 |= ADC_CR2_CAL;
	while (!(ADC1->CR2 & ADC_CR2_CAL));

	ADC1->CR2 |= ADC_CR2_EXTSEL_2;												// EXTSEL=100: Timer 3 TRGO event
	ADC1->CR2 |= ADC_CR2_EXTTRIG;  						  				// включаем запуск от внешнего события
	ADC1->SMPR1 |= ADC_SMPR1_SMP10_2 ; 										// 1xx
	ADC1->SMPR1 &= ~ADC_SMPR1_SMP10_0; 										// 1x0
	ADC1->SMPR1 &= ~ADC_SMPR1_SMP10_1; 										// 100

	ADC1->CR2 |= ADC_CR2_DMA;															// включаю DMA
	
}
