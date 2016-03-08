#include "main.h"
#include "math.h"
#define M_PI 3.14159265359
#define f_search 17100
#define f_search1 17500
#define N 147
int level=50000;
int level1=50000;
int i;
int n=0;
int j=0;
uint16_t ADC_PREV[74];
uint16_t ADC;

extern uint16_t ADC_DATA[147];
unsigned char detected_position=0;
unsigned char offset=0;
unsigned char detected=0;
float u,u_adc,u1;
float I[2];
float Q[2];
float I1[2];
float Q1[2];
float r;
float rN;
float module, module2,module1;
float a,a1;
float b,b1;
float k0;
float Fd;
int digit;
int num,num1;
int nn;
void DMA1_Channel1_IRQHandler ()
{
	
	if (DMA1->ISR & DMA_ISR_HTIF1) //Half Transfer Interrupt
	{
		n++;
		for (i=0; i<74; i++)
		{
			I[0] = I[1];
			Q[0] = Q[1];
			I1[0] = I1[1];
			Q1[0] = Q1[1];
			if (!detected) 
			{
				u_adc=(float)(ADC_DATA[i]) - rN*(float)ADC_PREV[i];
				u= (float)I[0] + u_adc;
				u1=(float)I1[0] + u_adc;
			}
			else 
			{
				u = (float)I[0] + (float)(ADC_DATA[i]);
				u1 = (float)I1[0] + (float)(ADC_DATA[i]);
			}
				
			I[1] = (u*a - b*Q[0]);
			Q[1] = (u*b + a*Q[0]);
			I1[1] = (u1*a1 - b1*Q1[0]);
			Q1[1]  = (u1*b1 + a1*Q1[0]);
			
			if ((I[1]>=0)&&(Q[1]>=0))
			{
				module = I[1]+Q[1];
			}
			else if	((I[1]<0)&&(Q[1]>=0))
			{
				module = Q[1]-I[1];
			}
			else if	((I[1]>=0)&&(Q[1]<0))
			{
				module = I[1]-Q[1];
			}
				
			
			//module1=sqrt(I1[1]*I1[1]+Q1[1]*Q1[1]);
			
			if ((module>level)||(module1>level1))
			{	
				detected_position=i;
				detected=1;
				if(module>level) 
				{
					I[0]=0;
					I[1]=0;
					Q[0]=0;
					Q[1]=0;
					if (!((num+num1)%10)) digit=0;
					num++;
					digit=digit<<1;
				}
				if (module1>level1) 
				{
					I1[0]=0;
					I1[1]=0;
					Q1[0]=0;
					Q1[1]=0;
					if (!((num+num1)%10)) digit=0;
					num1++;
					digit=digit<<1;
					digit|=1;
					
				}
				
			}
			
			if (detected) 
			{
				offset++;			//offset (0..detected_position) <147
				if (offset>detected_position) 
				{
					detected=0;
					offset=0;
				}
			}
		}
		for (i=74; i<147; i++)
		{
			ADC_PREV[i-74]=((ADC_DATA[i]));
		}	
	}
	if (DMA1->ISR & DMA_ISR_TCIF1) //Transfer Complete Interrupt
	{
		n--;
		for (i=74; i<147; i++)
		{
		  
			I[0] = I[1];
			Q[0] = Q[1];
			I1[0] = I1[1];
			Q1[0] = Q1[1];
			if (!detected) 
			{
				u_adc=(float)(ADC_DATA[i]) - rN*(float)ADC_PREV[i];
				u= (float)I[0] + u_adc;
				u1=(float)I1[0] + u_adc;		
			}
			else 
			{
				u = (float)I[0] + (float)(ADC_DATA[i]);
				u1 = (float)I1[0] + (float)(ADC_DATA[i]);
			}
			I[1] = (u*a - b*Q[0]);
			Q[1] = (u*b + a*Q[0]);
			I1[1] = (u1*a1 - b1*Q1[0]);
			Q1[1]  = (u1*b1 + a1*Q1[0]);
			
			if ((I[1]>=0)&&(Q[1]>=0))
			{
				module = I[1]+Q[1];
			}
			else if	((I[1]<0)&&(Q[1]>=0))
			{
				module = Q[1]-I[1];
			}
			else if	((I[1]>=0)&&(Q[1]<0))
			{
				module = I[1]-Q[1];
			}
			
			if ((I1[1]>=0)&&(Q1[1]>=0))
			{
				module1 = I1[1]+Q1[1];
			}
			else if	((I1[1]<0)&&(Q1[1]>=0))
			{
				module1 = Q1[1]-I1[1];
			}
			else if	((I1[1]>=0)&&(Q1[1]<0))
			{
				module1 = I1[1]-Q1[1];
			}
			//module1=sqrt(I1[1]*I1[1]+Q1[1]*Q1[1]);
			
			if ((module>level)||(module1>level1))
			{
			
		
				detected_position=i;
				detected=1;
				if(module>level) 
				{
					I[0]=0;
					I[1]=0;
					Q[0]=0;
					Q[1]=0;
					if (!((num+num1)%10)) digit=0;
					num++;
					digit=digit<<1;
				}
				if (module1>level1) 
				{
					I1[0]=0;
					I1[1]=0;
					Q1[0]=0;
					Q1[1]=0;
					if (!((num+num1)%10)) digit=0;
					num1++;
					digit=digit<<1;
					digit|=1;
					
				}
				
			}
			if (detected) 
			{
				offset++;			//offset (0..detected_position) <147
				if (offset>detected_position) 
				{
					detected=0;
					offset=0;
				}
			}
		}
		for (i=0; i<74; i++)
		{
			ADC_PREV[i]=((ADC_DATA[i])>>4);
		}

		
		if (n==12) n=0;
	}
	
	DMA1->IFCR|= DMA_IFCR_CGIF1;
}


int main (void)
{	
	num=0;
	//generating test sinus in array adc[]
/*for (i =0; i<2000; i++) 
{
    if ((i/147)%2)
    {
        adc[i] = (unsigned char) 50*sin((2*M_PI*17000*i/44100 + M_PI/5))+200;
    }
    else
    {
        adc[i]= (unsigned char) 100*sin((2*M_PI*5000*i/44100))+100;;
    }
}
	*/
	Fd=72000000.0/1633.0;
	r=0.9995;
	rN=pow(r,N);
	k0= (float)f_search / ((float)Fd/(float)N);
	a=r*cos(2*M_PI*k0/N);
	b=r*sin(2*M_PI*k0/N);
	k0= (float)f_search1 / ((float)Fd/(float)N);
	a1=r*cos(2*M_PI*k0/N);
	b1=r*sin(2*M_PI*k0/N);
	GPIO_init();
	ADC_init();
	TIM_init();
	DMA_init();

	while (1)
	{
		

		
			
	}
	
}
