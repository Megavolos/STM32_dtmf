#include "sdft.h"

extern uint16_t ADC_DATA[147];			//массив, куда DMA кладет семплы
SDFT_VARS sdft_vars;

void sdft_init(SDFT_VARS* sdft_vars_ptr)
{
	unsigned char i;
	//обнуляем на всякий случай все переменные
	sdft_vars_ptr->offset=0;
	sdft_vars_ptr->detected=0;
	sdft_vars_ptr->detected_position=0;
	sdft_vars_ptr->detected_freq=0;
	sdft_vars_ptr->rN=pow(r,N); 										//вычисление r^N
	
	for (i=0;i<8;i++)
	{
		//F_LOW+F_STEP*i = искомая частота 
		sdft_vars_ptr->a[i]=r*cos(2*M_PI*((float)(F_LOW+F_STEP*i) / ((float)Fd/(float)N))/N);
		sdft_vars_ptr->b[i]=r*sin(2*M_PI*((float)(F_LOW+F_STEP*i) / ((float)Fd/(float)N))/N);
		sdft_vars_ptr->module[i]=0;										//заодно обнуляем модуль на всякий случай
		sdft_vars_ptr->I[i*2]=0;											//заодно обнуляем все четные (предыдущие) I
		sdft_vars_ptr->I[(i*2)+1]=0;									//заодно обнуляем все нечетные  (текущие) I
		sdft_vars_ptr->Q[i*2]=0;											//заодно обнуляем все четные (предыдущие) Q
		sdft_vars_ptr->Q[(i*2)+1]=0;									//заодно обнуляем все нечетные  (текущие) Q
		sdft_vars_ptr->level[i]=50000;								//если вдруг не установили уровни, то будут 50000
	}
}

void store_IQs(SDFT_VARS* sdft_vars_ptr)						//сохраняем текущие значения I и Q в предыдущие значения I и Q
{
	//для увеличения быстродействия производится прямое присванивание без вычислений индекса
	sdft_vars_ptr->I[0]=sdft_vars_ptr->I[1];				//для f_search[0]
	sdft_vars_ptr->Q[0]=sdft_vars_ptr->Q[1];
	sdft_vars_ptr->I[2]=sdft_vars_ptr->I[3];				//для f_search[1]
	sdft_vars_ptr->Q[2]=sdft_vars_ptr->Q[3];
	sdft_vars_ptr->I[4]=sdft_vars_ptr->I[5];				//для f_search[2]
	sdft_vars_ptr->Q[4]=sdft_vars_ptr->Q[5];
	sdft_vars_ptr->I[6]=sdft_vars_ptr->I[7];				//для f_search[3]
	sdft_vars_ptr->Q[6]=sdft_vars_ptr->Q[7];
	sdft_vars_ptr->I[8]=sdft_vars_ptr->I[9];				//для f_search[4]
	sdft_vars_ptr->Q[8]=sdft_vars_ptr->Q[9];
	sdft_vars_ptr->I[10]=sdft_vars_ptr->I[11];			//для f_search[5]
	sdft_vars_ptr->Q[10]=sdft_vars_ptr->Q[11];
	sdft_vars_ptr->I[12]=sdft_vars_ptr->I[13];			//для f_search[6]
	sdft_vars_ptr->Q[12]=sdft_vars_ptr->Q[13];
	sdft_vars_ptr->I[14]=sdft_vars_ptr->I[15];			//для f_search[7]
	sdft_vars_ptr->Q[14]=sdft_vars_ptr->Q[15];
}
unsigned char compare_module_with_level(SDFT_VARS* sdft_vars_ptr, unsigned int module_num )
{
	unsigned char detected_local=0;
	unsigned char current = module_num*2+1;
	if (!sdft_vars_ptr->detected)							//если detected=0,т.е. после детектирования прошло detected_position семплов
	{
		//вместо вычисления корня из суммы квадратов, просто складываю по модулю I и Q
		if ((sdft_vars_ptr->I[current]>=0)&&(sdft_vars_ptr->Q[current]>=0))
		{
			sdft_vars_ptr->module[module_num] = sdft_vars_ptr->I[current]+sdft_vars_ptr->Q[current];
		}
		else if	((sdft_vars_ptr->I[current]<0)&&(sdft_vars_ptr->Q[current]>=0))
		{
			sdft_vars_ptr->module[module_num] = sdft_vars_ptr->Q[current]-sdft_vars_ptr->I[current];
		}
		else if	((sdft_vars_ptr->I[current]>=0)&&(sdft_vars_ptr->Q[current]<0))
		{
			sdft_vars_ptr->module[module_num] = sdft_vars_ptr->I[current]-sdft_vars_ptr->Q[current];
		}
	}
	if (sdft_vars_ptr->module[module_num]>sdft_vars_ptr->level[module_num])
	{
		sdft_vars_ptr->detected_position=sdft_vars_ptr->pos;
		sdft_vars_ptr->detected=1;
		detected_local=1;
		sdft_vars_ptr->I[current-1]=0;
		sdft_vars_ptr->I[current]=0;
		sdft_vars_ptr->Q[current-1]=0;
		sdft_vars_ptr->Q[current]=0;
	}
	if (sdft_vars_ptr->detected) 
	{
		sdft_vars_ptr->offset++;			//offset (0..detected_position) <147
		if (sdft_vars_ptr->offset > sdft_vars_ptr->detected_position) 
		{
			sdft_vars_ptr->detected=0;
			sdft_vars_ptr->offset=0;
		}
	}
	return detected_local;
}
unsigned char search_freq(SDFT_VARS* sdft_vars_ptr)
{
	//no FOR cycle for economy CPU time
	sdft_vars_ptr->I[1] = sdft_vars_ptr->u[0]*sdft_vars_ptr->a[0] - sdft_vars_ptr->b[0]*sdft_vars_ptr->Q[0];
	sdft_vars_ptr->Q[1] = sdft_vars_ptr->u[0]*sdft_vars_ptr->b[0] + sdft_vars_ptr->a[0]*sdft_vars_ptr->Q[0];
	if (compare_module_with_level(sdft_vars_ptr,0)) sdft_vars_ptr->detected_freq|=1;
	
	sdft_vars_ptr->I[3] = sdft_vars_ptr->u[1]*sdft_vars_ptr->a[1] - sdft_vars_ptr->b[1]*sdft_vars_ptr->Q[2];
	sdft_vars_ptr->Q[3] = sdft_vars_ptr->u[1]*sdft_vars_ptr->b[1] + sdft_vars_ptr->a[1]*sdft_vars_ptr->Q[2];
	if (compare_module_with_level(sdft_vars_ptr,1)) sdft_vars_ptr->detected_freq|=(1<<1);
	
	sdft_vars_ptr->I[5] = sdft_vars_ptr->u[2]*sdft_vars_ptr->a[2] - sdft_vars_ptr->b[2]*sdft_vars_ptr->Q[4];
	sdft_vars_ptr->Q[5] = sdft_vars_ptr->u[2]*sdft_vars_ptr->b[2] + sdft_vars_ptr->a[2]*sdft_vars_ptr->Q[4];
	if (compare_module_with_level(sdft_vars_ptr,2)) sdft_vars_ptr->detected_freq|=(1<<2);
	
	sdft_vars_ptr->I[7] = sdft_vars_ptr->u[3]*sdft_vars_ptr->a[3] - sdft_vars_ptr->b[3]*sdft_vars_ptr->Q[6];
	sdft_vars_ptr->Q[7] = sdft_vars_ptr->u[3]*sdft_vars_ptr->b[3] + sdft_vars_ptr->a[3]*sdft_vars_ptr->Q[6];
	if (compare_module_with_level(sdft_vars_ptr,3)) sdft_vars_ptr->detected_freq|=(1<<3);
	
	sdft_vars_ptr->I[9] = sdft_vars_ptr->u[4]*sdft_vars_ptr->a[4] - sdft_vars_ptr->b[4]*sdft_vars_ptr->Q[8];
	sdft_vars_ptr->Q[9] = sdft_vars_ptr->u[4]*sdft_vars_ptr->b[4] + sdft_vars_ptr->a[4]*sdft_vars_ptr->Q[8];
	if (compare_module_with_level(sdft_vars_ptr,4)) sdft_vars_ptr->detected_freq|=(1<<4);
	
	sdft_vars_ptr->I[11] = sdft_vars_ptr->u[5]*sdft_vars_ptr->a[5] - sdft_vars_ptr->b[5]*sdft_vars_ptr->Q[10];
	sdft_vars_ptr->Q[11] = sdft_vars_ptr->u[5]*sdft_vars_ptr->b[5] + sdft_vars_ptr->a[5]*sdft_vars_ptr->Q[10];
	if (compare_module_with_level(sdft_vars_ptr,5)) sdft_vars_ptr->detected_freq|=(1<<5);
	
	sdft_vars_ptr->I[13] = sdft_vars_ptr->u[6]*sdft_vars_ptr->a[6] - sdft_vars_ptr->b[6]*sdft_vars_ptr->Q[12];
	sdft_vars_ptr->Q[13] = sdft_vars_ptr->u[6]*sdft_vars_ptr->b[6] + sdft_vars_ptr->a[6]*sdft_vars_ptr->Q[12];
	if (compare_module_with_level(sdft_vars_ptr,6)) sdft_vars_ptr->detected_freq|=(1<<6);
	
	sdft_vars_ptr->I[15] = sdft_vars_ptr->u[7]*sdft_vars_ptr->a[7] - sdft_vars_ptr->b[7]*sdft_vars_ptr->Q[14];
	sdft_vars_ptr->Q[15] = sdft_vars_ptr->u[7]*sdft_vars_ptr->b[7] + sdft_vars_ptr->a[7]*sdft_vars_ptr->Q[14];
	if (compare_module_with_level(sdft_vars_ptr,7)) sdft_vars_ptr->detected_freq|=(1<<7);
}

void get_module (SDFT_VARS* sdft_vars_ptr, unsigned char TCI)
{
	//TCI=0 => pos_begin_shift= 0, pos_end_shift= 74, adc_store_pos=74
	//TCI=1 => pos_begin_shift=74, pos_end_shift=147, adc_store_pos=0
	//
	unsigned char pos_begin_shift=TCI*74;						
	unsigned char pos_end_shift=HALF_OF_N+TCI*73;
	unsigned char adc_store_pos=74-TCI*(74);
	float u_temp;
	float u;
	
	store_IQs(sdft_vars_ptr);
	
	if (!sdft_vars_ptr->detected) 																			//если detected=0
	{
		for (sdft_vars_ptr->pos=pos_begin_shift; sdft_vars_ptr->pos < pos_end_shift; sdft_vars_ptr->pos++)
		{
			u_temp=(float)(ADC_DATA[sdft_vars_ptr->pos]) - sdft_vars_ptr->rN * (float)sdft_vars_ptr->ADC_PREV[sdft_vars_ptr->pos];
			sdft_vars_ptr->u[0]=(float)sdft_vars_ptr->I[0] + u_temp;				//freq = FREQ_LOW
			sdft_vars_ptr->u[1]=(float)sdft_vars_ptr->I[2] + u_temp;
			sdft_vars_ptr->u[2]=(float)sdft_vars_ptr->I[4] + u_temp;
			sdft_vars_ptr->u[3]=(float)sdft_vars_ptr->I[6] + u_temp;
			sdft_vars_ptr->u[4]=(float)sdft_vars_ptr->I[8] + u_temp;
			sdft_vars_ptr->u[5]=(float)sdft_vars_ptr->I[10] + u_temp;
			sdft_vars_ptr->u[6]=(float)sdft_vars_ptr->I[12] + u_temp;
			sdft_vars_ptr->u[7]=(float)sdft_vars_ptr->I[14] + u_temp;
			search_freq(sdft_vars_ptr);
			//TCI=0 => ADC_PREV[0]..ADC_PREV[73] = ADC_DATA[74]..ADC_DATA[146]
			//TCI=1 => ADC_PREV[0]..ADC_PREV[73] = ADC_DATA[ 0]..ADC_DATA[ 73]
			sdft_vars_ptr->ADC_PREV[sdft_vars_ptr->pos]=((ADC_DATA[sdft_vars_ptr->pos+adc_store_pos]));
		}
	}
	else 																																//если detected=1
	{
		for (sdft_vars_ptr->pos=(0+TCI*74); sdft_vars_ptr->pos<(HALF_OF_N+TCI*73); sdft_vars_ptr->pos++)
		{
			sdft_vars_ptr->u[0] = (float)sdft_vars_ptr->I[0] + (float)(ADC_DATA[sdft_vars_ptr->pos]);
			sdft_vars_ptr->u[1] = (float)sdft_vars_ptr->I[2] + (float)(ADC_DATA[sdft_vars_ptr->pos]);
			sdft_vars_ptr->u[2] = (float)sdft_vars_ptr->I[0] + (float)(ADC_DATA[sdft_vars_ptr->pos]);
			sdft_vars_ptr->u[3] = (float)sdft_vars_ptr->I[2] + (float)(ADC_DATA[sdft_vars_ptr->pos]);
			sdft_vars_ptr->u[4] = (float)sdft_vars_ptr->I[0] + (float)(ADC_DATA[sdft_vars_ptr->pos]);
			sdft_vars_ptr->u[5] = (float)sdft_vars_ptr->I[2] + (float)(ADC_DATA[sdft_vars_ptr->pos]);
			sdft_vars_ptr->u[6] = (float)sdft_vars_ptr->I[0] + (float)(ADC_DATA[sdft_vars_ptr->pos]);
			sdft_vars_ptr->u[7] = (float)sdft_vars_ptr->I[2] + (float)(ADC_DATA[sdft_vars_ptr->pos]);
			search_freq(sdft_vars_ptr);
			//TCI=0 => ADC_PREV[0]..ADC_PREV[73] = ADC_DATA[74]..ADC_DATA[146]
			//TCI=1 => ADC_PREV[0]..ADC_PREV[73] = ADC_DATA[ 0]..ADC_DATA[ 73]
			sdft_vars_ptr->ADC_PREV[sdft_vars_ptr->pos]=((ADC_DATA[sdft_vars_ptr->pos+adc_store_pos]));
		}
	}
		
	
					
				
}