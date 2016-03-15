#ifndef SDFT_H
#define SDFT_H
/*
#include "stm32f10x.h"
#include "math.h"

#define F_CPU 					72000000					//частота микроконтроллера
#define TIM_ARR 				1633							//до куда считает таймер
#define Fd 							F_CPU/TIM_ARR			//получаемая частота дискретизации,исходя из настроек таймера
#define M_PI 						3.14159265359			//число Пи
#define N 							147								//длина скользящего окна
#define HALF_OF_N 			74								//длина половины скользящего окна (если окно нечет., то длина наибольшей половины)

#define F_LOW						17100							//нижняя частота DTMF
#define F_STEP					300								//шаг частот DTMF

#define r								0.9995						//коэффициент затухания, необходим из-за конечной разрядной сетки (ошибки округления)


typedef struct
{

	//unsigned int f_search[8];								// массив с искомыми частотами DTMF
	unsigned int level[8];									// массив с уровнями порога срабатывания по модулю
	uint16_t ADC_DATA[N];										// массив, куда DMA складывает семплы
	unsigned char ADC_PREV[HALF_OF_N];			// массив, в котором хранится предыдущая половина передачи
	unsigned char detected_position;				// позиция семпла в буфере, на которой произошло срабатывание по порогу
	unsigned char offset;										// смещение от этой позиции
	unsigned char detected;									// логическая переменная. 1 если произошло детектирование
	unsigned char pos; 											// номер текущего семпла
	unsigned char detected_freq;					//8-бит значение. Каждый разряд отвечает за частоту, например 0001 0010 - детектированы частоты f1 и f4
	unsigned char pos_begin_shift;
	unsigned char pos_end_shift;
	unsigned char adc_store_pos;
	float I[16];														// Re от результата SDFT для 8 частот. Нечетные - предыдущие, четные - текущие
	float Q[16];														// Im от результата SDFT для 8 частот. Нечетные - предыдущие, четные - текущие
	float a[8];															// cos (Re) поворачивающего множителя для искомых 8 частот
	float b[8];															// sin (Re) поворачивающего множителя для искомых 8 частот
	float module[8];												// модули искомых частот
	float rN;																// r^N
	float u[8];															// переменная, в которой хранится часть рассчетов
} SDFT_VARS;

void sdft_init(SDFT_VARS* sdft_vars_ptr);
void store_IQs(SDFT_VARS* sdft_vars_ptr);
unsigned char compare_module_with_level(SDFT_VARS* sdft_vars_ptr, unsigned int module_num );
void search_freq(SDFT_VARS* sdft_vars_ptr);
void detect_freq (SDFT_VARS* sdft_vars_ptr, unsigned char TCI);

*/
#endif 
