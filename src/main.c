#include "main.h"
#include "math.h"
#define M_PI 3.14159265359
#define SAMPLING_RATE	44100	//8kHz
#define FIRST_FREQUENCY	17100	//941 Hz
#define STEP 300
#define N	147	//Block size
#define MULT 65536
extern uint16_t ADC_DATA[N];

int coeff[8];
int32_t Q1[8];
int32_t Q2[8];
float sine;
float cosine;
int32_t module[8],module_prev,result;
unsigned char i;
unsigned char num[8];
unsigned char first_half[8];
unsigned char digit[100];
unsigned char digit_pos;
unsigned char detected_prev[8];
float level=800000;
float level2=800000;
float level3=10000;
unsigned char n;
unsigned test=0;
unsigned char detected[8];
unsigned char sample;
unsigned int detected_pos[8];
unsigned char first_half[8];
void ResetGoertzel(unsigned char freq_num)
{
  Q2[freq_num] = 0;
  Q1[freq_num] = 0;
}
void InitGoertzel(unsigned char freq_num)
{
  int	k;
//  float	floatN;
  float	omega;

  //floatN = (float) N;
  k = (int) (0.5 + (((float)N * (float)(FIRST_FREQUENCY+freq_num*STEP)) / (float)SAMPLING_RATE));
  omega = (float)(2.0 * M_PI * (float)k) /(float)N;
  sine = sin(omega);
  cosine = cos(omega);

  coeff[freq_num] = (int)(2.0 * cosine*MULT +0.5);
	

  ResetGoertzel(freq_num);
}
void ProcessSample(unsigned char freq_num, unsigned char sample)
{
  int Q0;
  Q0 = coeff[freq_num] * Q1[freq_num];
	Q0 -= (Q2[freq_num]*MULT);
	Q0 += (sample*MULT);
  Q2[freq_num] = Q1[freq_num];
  Q1[freq_num] = Q0/MULT;
}
int32_t GetMagnitudeSquared(unsigned char freq_num)
{
  int32_t result;
	result = Q1[freq_num] * Q1[freq_num] + Q2[freq_num] * Q2[freq_num] - Q1[freq_num] * Q2[freq_num] * (coeff[freq_num]/MULT);
  return result;
}
unsigned char CompareModule(unsigned char freq_num)
{
	
	if (first_half[freq_num]==1)
		{
			module[freq_num] = GetMagnitudeSquared(freq_num);
			ResetGoertzel(freq_num);
			first_half[freq_num]=0;
			
		}
	if ((GetMagnitudeSquared(freq_num)>level3))
		{	
			first_half[freq_num]=1;	
		}
	else
		{
			ResetGoertzel(freq_num);
			first_half[freq_num]=0;	 
		}	
	detected_prev[freq_num]=detected[freq_num];
	if ((module[freq_num]>level))
		{
			
			
			//if (freq_num==3) num[3]++;
			//if (freq_num==4) num[4]++;
			//if (freq_num==7) num[7]++;
			//if (freq_num==3) GPIOC->ODR^= GPIO_ODR_ODR7;
			//if (freq_num==2){GPIOC->ODR^= GPIO_ODR_ODR6;} 
			//if (freq_num==7){GPIOC->ODR^= GPIO_ODR_ODR7;} 
			detected_pos[freq_num]=n;
			detected[freq_num]=1;
			module[freq_num]=0;
				
			
			return 1;
		}
	else
		{
			detected[freq_num]=0;
			
		//	if (freq_num==4) {GPIOC->BSRR= GPIO_BSRR_BR6;}
		//	if (freq_num==4){GPIOC->BSRR= GPIO_BSRR_BR7;} 
			return 0;
		}
}
void DMA1_Channel1_IRQHandler ()
{
	
	/*if (DMA1->ISR & DMA_ISR_HTIF1) //Half Transfer Interrupt
	{
		test++;
		for (i=0;i<147;i++)
		{
			sample=ADC_DATA[i]>>4;
			ProcessSample(0,sample);
			ProcessSample(1,sample);
			ProcessSample(2,sample);
			ProcessSample(3,sample);
			ProcessSample(4,sample);
			ProcessSample(5,sample);
			ProcessSample(6,sample);
			ProcessSample(7,sample);
		}
	
	}*/
	if (DMA1->ISR & DMA_ISR_TCIF1) //Transfer Complete Interrupt
	{
		
		for (i=0;i<147;i++)
		{
			sample=ADC_DATA[i]>>4;
			ProcessSample(0,sample);
			ProcessSample(1,sample);
			ProcessSample(2,sample);
			ProcessSample(3,sample);
			ProcessSample(4,sample);
			ProcessSample(5,sample);
			ProcessSample(6,sample);
			ProcessSample(7,sample);
		}
		
		
		CompareModule(0); //GPIOC->ODR^= GPIO_ODR_ODR6; 				//17100
		CompareModule(1);// GPIOC->ODR^= GPIO_ODR_ODR7; 				//17400
		CompareModule(2); //GPIOD->ODR^= GPIO_ODR_ODR13;  		//17700
		CompareModule(3);   //18000
		CompareModule(4); 				//18300
		CompareModule(5);
		CompareModule(6);
		CompareModule(7);
		
		//if (detected[3]) {GPIOC->BSRR= GPIO_BSRR_BS6;} else {GPIOC->BSRR= GPIO_BSRR_BR6;}
		//if (detected[7]) GPIOC->ODR^= GPIO_ODR_ODR7;
		
		if (detected[4]) //18300
		{
			
			if (detected[0]) {digit[digit_pos]='0'; digit_pos++;}
			if (detected[1]) {digit[digit_pos]='1'; digit_pos++;}
			if (detected[2]) {digit[digit_pos]='2'; digit_pos++;}
			if (detected[3]) {digit[digit_pos]='3'; digit_pos++;GPIOD->ODR^= GPIO_ODR_ODR13;}
		}
	
		if (detected[5]) //18700
		{
			if (detected[0]) {digit[digit_pos]='4'; digit_pos++;}
			if (detected[1]) {digit[digit_pos]='5'; digit_pos++;}
			if (detected[2]) {digit[digit_pos]='6'; digit_pos++;}
			if (detected[3]) {digit[digit_pos]='7'; digit_pos++;}
		}
		
		if (detected[6]) //18700
		{
			if (detected[0]) {digit[digit_pos]='8'; digit_pos++;}
			if (detected[1]) {digit[digit_pos]='9'; digit_pos++;}
			if (detected[2]) {digit[digit_pos]='A'; digit_pos++;}
			if (detected[3]) {digit[digit_pos]='B'; digit_pos++;}
		}
		if (detected[7]) //18700
		{
			if (detected[0]) {digit[digit_pos]='C'; digit_pos++;}
			if (detected[1]) {digit[digit_pos]='D'; digit_pos++;}
			if (detected[2]) {digit[digit_pos]='E'; digit_pos++;}
			if (detected[3]) {digit[digit_pos]='F'; digit_pos++;GPIOD->ODR^= GPIO_ODR_ODR6;}
		}
		if (digit_pos>=16) digit_pos=0;
		n++;
	}
	
	DMA1->IFCR|= DMA_IFCR_CGIF1;
	
}


int main (void)
{	
	unsigned char _i;

	GPIO_init();
	ADC_init();
	TIM_init();
	DMA_init();
	InitGoertzel(0);
	InitGoertzel(1);
	InitGoertzel(2);
	InitGoertzel(3);
	InitGoertzel(4);
	InitGoertzel(5);
	InitGoertzel(6);
	InitGoertzel(7);
	for (_i=0;_i<8;_i++)
	{
		detected_pos[i]=0;
		detected[i]=0;
	}
	while (1)
	{
		

		
			
	}
	
}
