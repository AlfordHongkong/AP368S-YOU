#include "myboard.h"
#include "ap368s.h"
volatile TIMBITS 	time_half_sec;
volatile TIMBITS	timer_10ms_flag;
volatile TIMBITS  	timer_500ms_flag;
volatile TIMBITS  	timer_50ms_flag;
volatile TIMBITS  	timer_min_flag;
volatile TIMBITS  	timer_1ms_flag;

void (*timer1_int_srv)(void)=0;
void (*timer2_int_srv)(void)=0;
void (*timer3_int_srv)(void)=0;
void (*timer4_int_srv)(void)=0;


const TIMBITS half_val = 1<<(sizeof(TIMBITS)*8-1);
U8	cnt_10ms;
U8	cnt_50ms;
U8	cnt_500ms;

static BOOL test_timer(volatile TIMBITS* flag,TIMBITS mask);



void	init_timer1(void)
{

}



//----------------------------------------------
static BOOL test_timer(volatile TIMBITS* flag,TIMBITS mask)
{
	if(flag[0] & mask)
	{
		flag[0] ^=mask;
		return TRUE;
	}
	else{
		return FALSE;
	}
}
//----------------------------------------------
BOOL is_50ms_int(TIMBITS timer_mask)
{
	return test_timer(&timer_50ms_flag,timer_mask);
}

BOOL is_10ms_int(TIMBITS timer_mask)
{
	return test_timer(&timer_10ms_flag,timer_mask);
}
//----------------------------------------------
BOOL is_500ms_sync(void)
{
	return  (timer_500ms_flag & half_val) != 0 ;
}
//----------------------------------------------
BOOL is_1ms_int(TIMBITS timer_mask)
{
	return test_timer(&timer_1ms_flag,timer_mask);
}
//----------------------------------------------
BOOL is_500ms_int(TIMBITS timer_mask)
{
	return test_timer(&timer_500ms_flag,timer_mask);
}
//-----------------------------------------------
//----------------------------------------------
void set_half_sec_cnt(U8 val)
{
	time_half_sec = val;
}

void delay_loop(U32 loop_num)
{
	//delay 1us. at 8Mhz.

	while(loop_num){	
		loop_num--;		
		__NOP();
		__NOP();		
		__NOP();		
		__NOP();		
	}
}

		/* system running time */
void HAL_SYSTICK_Callback(void)
{
	extern void uart_1ms_int_srv(void);
	uart_1ms_int_srv();
	key_scan_1ms_int();
	key_srv_1ms_int();
	if(is_power_on()){
		ADC1->CR2 |= ADC_CR2_SWSTART;
	}
	timer_1ms_flag = (TIMBITS)~0u;
	cnt_10ms++;
	if(cnt_10ms >= 10){
		cnt_10ms =0;
		timer_10ms_flag = (TIMBITS)~0u;
		cnt_50ms++;
		if(cnt_50ms == 5){
			cnt_50ms = 0;
			timer_50ms_flag = (TIMBITS)~0u;
			cnt_500ms++;
			if(cnt_500ms == 10){
				//xor heighest bit, set other bits.
				cnt_500ms = 0;
				timer_500ms_flag ^= half_val;
				timer_500ms_flag  |= half_val-1;
			}
		}
	}
}

//////////////////////////////////////////////////////////
//-------------------------------------------------
void delay_ms(U16 time_ms)
{
	while(time_ms){
		IWDG->KR = 0xAAAA;	//clear
		delay_loop(1000);
		time_ms--;
	}
}
//////////////////////////////////////////////////////////
			/* µ÷ÕûÁÁ¶È */
void TIM3_IRQHandler(void)
{
	U32 SR;
	SR = TIM3->SR;
			/* compare interrupt */
	if(SR & TIM_SR_CC1IF){
		TIM3->SR &=  ~(TIM_SR_UIF|TIM_SR_CC1IF) ;
		write_pin(LED_POWER_PIN, 1);
		write_pin(LED_CLEAN_PIN, 1);
		write_pin(LED_DRYING_PIN, 1);
		write_pin(LED_HIGH_PIN, 1);
		write_pin(LED_MID_PIN, 1);
		write_pin(LED_LOW_PIN, 1);
	}		
			/* update interrupt */
	if(SR & TIM_SR_UIF){
		TIM3->SR &=  ~(TIM_SR_UIF|TIM_SR_CC1IF) ;
		write_pin(LED_POWER_PIN, led.power==0);
		write_pin(LED_CLEAN_PIN, led.clean==0);
		write_pin(LED_DRYING_PIN, led.drying==0);
		write_pin(LED_HIGH_PIN, led.high==0);
		write_pin(LED_MID_PIN, led.middle==0);
		write_pin(LED_LOW_PIN, led.low==0);
	}
}
void TIM4_IRQHandler(void)
{
	if(timer4_int_srv){
		timer4_int_srv();
	}else{
		TIM4->SR = 0;
	}
}
void TIM2_IRQHandler(void)
{
	if(timer2_int_srv){
		timer2_int_srv();
	}else{
		TIM2->SR = 0;
	}
}

//------------------------------------
void pwm_tim3_int_srv(void)
{
	//dbg_toggle();
	TIM3->SR = 0;
}
//------------------------------------
void timer1_stop_pwm(void)
{
	DMA1_Channel5->CCR &= ~DMA_CCR_EN;
	TIM1->DIER = 0;
	TIM1->CCR1 = 0;
	TIM1->BDTR = 0;
}

void timer1_init_pwm(unsigned freq, unsigned * buff, unsigned cnt,unsigned repeat)
{
	__TIM1_CLK_ENABLE();
	/* Peripheral interrupt init*/
	config_pin(PA8,GPIO_AF_OUT);
	TIM1->CR1 = 0;
	unsigned clk= 72*1000*1000;
	unsigned psc = 1;
	while( clk/psc/freq >65536){
		psc+=1;
	}
	TIM1->ARR = clk/psc/freq-1;
	TIM1->PSC= psc-1;

	TIM1->RCR = repeat;
	TIM1->CCR1 = 0;
	TIM1->SMCR = 0;
	TIM1->EGR = 0;
	
	TIM1->CR2 = TIM_CR2_CCDS;
	TIM1->CCMR1 = TIM_CCMR1_OC1PE | TIM_CCMR1_OC1FE | TIM_CCMR1_OC1M_0*6;
	TIM1->BDTR = 0;
	TIM1->CCER  = TIM_CCER_CC1E;
	TIM1->BDTR = TIM_BDTR_MOE;

	// set TIM1 update DMA channel 5
	//  MEM to PHI, PRI= heigh, 16bit++ to 16bit, no DMA INT
	DMA1_Channel5->CCR &= ~DMA_CCR_EN;
	DMA1_Channel5->CCR = DMA_CCR_DIR|DMA_CCR_CIRC|DMA_CCR_PL_1
		|DMA_CCR_MSIZE_0|DMA_CCR_PSIZE_0|DMA_CCR_MINC;
	DMA1_Channel5->CMAR = (uint32_t)buff;
	DMA1_Channel5->CNDTR = cnt;
	DMA1_Channel5->CPAR = (uint32_t)&TIM1->CCR1;
	DMA1_Channel5->CCR |= DMA_CCR_EN;
	
	TIM1->DCR = TIM_DCR_DBL_0*1 + TIM_DCR_DBA_0*(uint32_t)(&TIM1->CCR1 - &TIM1->CR1);
	TIM1->DIER |= TIM_DIER_UDE;	//enable timer1 DMA
	TIM1->CR1 = TIM_CR1_CEN;

}



