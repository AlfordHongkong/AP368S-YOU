#include "myboard.h"
#include "AP368S.h"


 SYS_STATUS ap368s;
LED_STATUS led;
volatile U16 adc_buff[4];		//adc value updated by  adc dma.

const U16 fan_pwm_data[]=
{	
	
	FAN_PWM_CNT_MAX+1,		//off
	FAN_PWM_CNT_MAX*(1-0.175),	//low   	vfan = 6V
	FAN_PWM_CNT_MAX*(1-0.26),	//middle 	vfan = 8v
	FAN_PWM_CNT_MAX*(1-0.785),	//high		vfan = 12v
	FAN_PWM_CNT_MAX*(1-0.898),	//boost		vfan = 13.8v
	FAN_PWM_CNT_MAX*(1-0.775)	//dry mode	vfan = 12v
};

 U16 adc2light_tab[]=
{
	//key, value.
	//adc, light_level  in descend order.  pwm ratio, max is 100.
	4096*0.3, 10,
	4096*0.4, 15,
	4096*0.5, 25,
	4096*0.6,  35,
	4096*0.65, 45,
	4096*0.7,  55,
	4096*0.75, 65,
	4096*0.85, 70,
	4096*0.90, 75,
	4096*0.95, 80,
	4096*0.97, 101,
	0,0,
}; 

//-----------------------------------
void adc_deinit(void)
{
	//stop adc dma.
	DMA1_Channel1->CCR = 0;		//stop previous ADC DMA first.
	ADC1->CR2 = 0;
	__HAL_RCC_ADC1_CLK_DISABLE();

}
//-----------------------------------
void adc_init(void)
{
	//adc ch1,ch4,ch6,ch17, sample time set to max
	//PHI to MEM  PRI= heigh, 16bit to 16bit, no DMA INT,circular, software trigger
	__HAL_RCC_DMA1_CLK_ENABLE();
	__HAL_RCC_ADC1_CLK_ENABLE();

	DMA1_Channel1->CCR = 0;		//stop previous ADC DMA first.
	DMA1->IFCR |= DMA_IFCR_CTEIF1|DMA_IFCR_CHTIF1 | DMA_IFCR_CTCIF1;	//clear DMA1 flags.
	DMA1_Channel1->CCR = DMA_CCR_PL_1|DMA_CCR_PL_0
		|DMA_CCR_MSIZE_0|DMA_CCR_PSIZE_0|DMA_CCR_MINC|DMA_CCR_CIRC;  // set o max priority, for sample interval may very short..
	DMA1_Channel1->CMAR = (uint32_t)adc_buff;
	DMA1_Channel1->CNDTR = 4;
	DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;
	DMA1_Channel1->CCR |= DMA_CCR_EN;

	ADC1->SMPR1 = 0;
	ADC1->SMPR2 = (ADC_SMPR2_SMP0_0*7<<1*3)| (ADC_SMPR2_SMP4_0*7<<4*3)|(ADC_SMPR2_SMP6_0*7<<6*3);
	ADC1->SMPR1 = (ADC_SMPR1_SMP17_0*7<<(17-10)*3); 
	ADC1->SQR3 = (ADC_SQR3_SQ1_0*1)| (ADC_SQR3_SQ2_0*4)| (ADC_SQR3_SQ3_0*6)|(ADC_SQR3_SQ4_0*17);
	ADC1->SQR2 = 0;
	ADC1->SQR1 = ADC_SQR1_L_0*(4-1); //4 channel

	ADC1->CR1 = ADC_CR1_DUALMOD_0*0| ADC_CR1_SCAN;
	ADC1->CR2 = ADC_CR2_TSVREFE|ADC_CR2_EXTTRIG|ADC_CR2_EXTSEL_0*7|ADC_CR2_DMA;	//software triggle
	ADC1->CR2 |= ADC_CR2_ADON;
}
//-----------------------------------
U16 find_led_light_level(U16 light_sensor_adc)
{
	return get_table_value(light_sensor_adc,adc2light_tab);
}

//-----------------------------------
void show_fan_led(U8 fan_level)
{
	U8 low,mid,high;
	low=mid=high=0;	

	switch(fan_level){
		case 4:
			high = mid = low = 1;
			break;		//boost mode.
		case 3:
			high =1;
			break;
		case 2:
			mid =1;
			break;
		case 1:
			low =1;
			break;
	}
	led.low = low;
	led.middle = mid;
	led.high = high;
}
//-----------------------------------------
void stop_fan(void)
{
	if(ap368s.fan_running){
		TIM4->CCR4 = FAN_PWM_CNT_MAX+1;	//ouput 0;
		delay_loop(100);		//wait a T to make new CCR take effect.	
		TIM4->CR1 = 0;
		ap368s.fan_running = 0;
		__TIM4_CLK_DISABLE();
	}
}
//-----------------------------------------
void start_fan(U8 fan_level)
{
	//tim4 ch4 as pwm
	if(ap368s.fan_running){
		//set pwm ratio and  indicate led.
		TIM4->CCR4 = fan_pwm_data[fan_level];
	}else{
		//init pwm 
		__TIM4_CLK_ENABLE();
		TIM4->CR1 = 0;
		TIM4->CR2 = 0;
		TIM4->SMCR = 0;
		TIM4->CCMR2 = TIM_CCMR2_OC4PE|TIM_CCMR2_OC4M_0*6;  //pwm mode1
		TIM4->EGR = 0;
		TIM4->BDTR = 0;

		TIM4->ARR = FAN_PWM_CNT_MAX;			// 8MHz clk, freq=24k	
		TIM4->PSC= 0;

		TIM4->CCR4 = fan_pwm_data[fan_level];
		TIM4->DIER = 0;
		TIM4->CCER  = TIM_CCER_CC4P|TIM_CCER_CC4E;
		TIM4->CR1 |= TIM_CR1_CEN;	
		ap368s.fan_running = 1;
	}	
}
//-----------------------------------------
void set_led_light_strength(U8 strength)
{
	if(strength >= LED_LIGHT_MAX){
		//set no pwm effect, keep no change 
		TIM3->CCR1 = TIM3->ARR+1;
	}else{
		TIM3->CCR1 = (U32)TIM3->ARR * strength/LED_LIGHT_MAX;	
	}
}
//-----------------------------------------
void init_led(void)
{
	//start led pwm timer control all led status.
	led.power = 0;
	led.drying = 0;
	led.clean = 0;
	led.high = 0;
	led.middle = 0;
	led.low = 0;
	led.light_level = 0;

	//start timer3 pwm.
	__TIM3_CLK_ENABLE();
	TIM3->CR1 = 0;
	TIM3->CR2 = 0;
	TIM3->SMCR = 0;
	TIM3->CCMR1 = TIM_CCMR1_OC1PE|TIM_CCMR1_OC1M_0*0;	//no output.
	TIM3->EGR = 0;
	TIM3->BDTR = 0;

	TIM3->ARR = 0xFFFE;				//note:just not set to 0xFFFE, make ARR+1 not overflow,
	//    ARR+1 is to set CCR for no pwm effect.
	//  8MHz, T=8.192ms	
	TIM3->PSC= 0;

	TIM3->CCR1 = (U32)TIM3->ARR * led.light_level/LED_LIGHT_MAX;
	TIM3->DIER = TIM_DIER_UIE| TIM_DIER_CC1IE; 
	TIM3->CCER  = 0;
	TIM3->BDTR = 0;	
	TIM3->CR1 |= TIM_CR1_CEN;
	HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
}
//-----------------------------------------
void deinit_led(void)
{
	//stop led pwm timer.
	led.power = 0;
	led.drying = 0;
	led.clean = 0;
	led.high = 0;
	led.middle = 0;
	led.low = 0;

	TIM3->CR1 = 0; //stop pwm 
	write_pin(LED_POWER_PIN, 1);
	write_pin(LED_CLEAN_PIN, 1);
	write_pin(LED_DRYING_PIN, 1);
	write_pin(LED_HIGH_PIN, 1);
	write_pin(LED_MID_PIN, 1);
	write_pin(LED_LOW_PIN, 1);
	__TIM3_CLK_DISABLE();
	
}

//-----------------------------------------
void set_power_led(U8 on)
{
	led.power = on;
}
//-----------------------------------------
void set_drying_led(U8 on)
{
	led.drying = on;
}
//-----------------------------------------
void set_clean_led(U8 on)
{
	led.clean = on;
}
//-----------------------------------------
void open_hivoltage(void)
{
	set_pin(HIVOLT_EBL_PIN);
}
//-----------------------------------------
void close_hivoltage(void)
{
	clr_pin(HIVOLT_EBL_PIN);
}
//---------------------------------------
void enter_clean_mode(void)
{
	ap368s.need_clean = 1;
	led.clean = 1;
}
//-----------------------------------------
void exit_clean_mode(void)
{
	extern U16 run_time;
	led.clean = 0;
	run_time = 0;
	store_running_time();	
	ap368s.need_clean = 0;
}

//-----------------------------------------
BOOL get_running_time(U16* run_time)
{
	//total 3 data byte, 1byte checksum = MSB+LSB+'A';
	U8 temp[3];

	//read address
	if(!eeprom_block_read(EEP_RUNTIME_ADDR,temp)){
		return 0;	//read failed
	}
	if(temp[2] != temp[0] + temp[1]+'A'){
		return 0;	//checksum error
	}
	*run_time = (temp[0]<<8) + temp[1];
	return 1;	
}
//-----------------------------------------
BOOL store_running_time(void)
{
	//total 3 data byte, 1byte checksum = MSB+LSB+'A';
	extern U16 run_time;

	U8 temp[3];
	
	temp[0]= run_time>>8;
	temp[1] = run_time;
	temp[2] = temp[0]+temp[1]+'A';
	return eeprom_block_write(EEP_RUNTIME_ADDR,temp);
}

BOOL  eeprom_init(void)
{
	extern U16 run_time;
	U8 temp[6];
	if(!eeprom_read(EEP_TEST_ADDR,temp,6)){
		return 0;
	}
	if(temp[0]== 'A' && temp[1]== 'P' && temp[2]== '3' && temp[3]== '6' 
		&& temp[4]== '8' && temp[5]== 'S' ){
			return 1;
	}
	return eeprom_lowlevel_init();
}
//----------------------------------------------
BOOL eeprom_lowlevel_init(void)
{
	extern U16 run_time;
	U8 temp[6];
	//is new eeprom.
	temp[0]= 'A';
	temp[1]='P';
	temp[2]= '3';
	temp[3]='6';
	temp[4]= '8';
	temp[5]='S';
	if(!eeprom_write(EEP_TEST_ADDR,temp,6)){
		return 0;
	}
	//init run_time to zero.
	eeprom_block_init(EEP_RUNTIME_ADDR,EEP_RUNTIME_BLK_NUM, EEP_RUNTIME_BLK_SIZE);
	run_time = 0;
	store_running_time();
	return 1;
}
