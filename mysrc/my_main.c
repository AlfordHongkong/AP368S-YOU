#include <string.h>
#include "myboard.h"
#include "eeprom.h"
#include "ap368s.h"

#define	HISTORY_ITEM_NUM		80

void running_time_srv(void);
void light_monitor_srv(void);
void led_powerup_test(void);
void fan_drying_srv(void);
void error_check_srv(void);
void system_srv(void);

extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef hadc1;
int error_cnt=0;
U8 temp[128];

U8 uart_tx2_buff[256];
U8 uart_rx2_buff[256];
CIRCULAR_BUFF uart2_rx_cir, uart2_tx_cir;
MYUART	MYUART2={USART2, 115200, &uart2_rx_cir, &uart2_tx_cir,};

U32 boost_mode_500ms_cnt=0;

U16 history_alm_volt[HISTORY_ITEM_NUM];
U16 history_curr_volt[HISTORY_ITEM_NUM];
U16 history_wt_idx;

S16 curr_delta;
S16 alm_delta;
U16 adc_aver_1v2= 4096*1200/3300;	//default   1.2/3.3*4096
U16 alm_volt_mv;
U16 curr_sensor_volt_mv;
U16 adc_aver_light;
S16 temperature;
S16 humidity;
U16 run_time;

const U16 pin_init_tab[][3]=
{ 
	//pin_name, value, config
	{KEY_POWER_PIN,		0,GPIO_INPUT_FLOATING},
	{KEY_MODE_PIN,		0,GPIO_INPUT_FLOATING},

	{LED_CLEAN_PIN,		1,GPIO_OUT_OPENDRAIN},
	{LED_DRYING_PIN,	1,GPIO_OUT_OPENDRAIN},
	{LED_POWER_PIN,		1,GPIO_OUT_OPENDRAIN},
	{LED_HIGH_PIN,		1,GPIO_OUT_OPENDRAIN},
	{LED_MID_PIN,		1,GPIO_OUT_OPENDRAIN},
	{LED_LOW_PIN,		1,GPIO_OUT_OPENDRAIN},

	{FAN_OPEN_PIN,		0,GPIO_AF_OUT},
	{HIVOLT_EBL_PIN,	0,GPIO_OUT},

	{ALARM_ADC_PIN,		0,GPIO_ANALOG},
	{CURRENT_ADC_PIN,	0,GPIO_ANALOG},
	{LIGHT_ADC_PIN,		0,GPIO_ANALOG},

	{HDC1080_CLK_PIN,	1,GPIO_OUT_OPENDRAIN},
	{HDC1080_SDA_PIN,	1,GPIO_OUT_OPENDRAIN},
	{EEPROM_CLK_PIN,	1,GPIO_OUT_OPENDRAIN},
	{EEPROM_SDA_PIN,	1,GPIO_OUT_OPENDRAIN},
	//init not used pin
	{PA0,	0,GPIO_ANALOG},
	{PA5,	0,GPIO_ANALOG},
	{PA7,	0,GPIO_ANALOG},

	{PB0,	0,GPIO_ANALOG},
	{PB10,	0,GPIO_INPUT_PULL_DOWN},
	{PB11,	0,GPIO_INPUT_PULL_DOWN},
	{PA9,	0, GPIO_INPUT_PULL_DOWN},
	{PA10,	0,GPIO_INPUT_PULL_DOWN},
	{PA15,	0,GPIO_INPUT_PULL_DOWN},
	{PB4,	0, GPIO_INPUT_PULL_DOWN},
	{PC13,	0,GPIO_INPUT_PULL_DOWN},
	{PC14,	0,GPIO_INPUT_PULL_DOWN},
	{PC15,	0, GPIO_INPUT_PULL_DOWN},
};

void cmd_proc(U8* cmd);
void uart_data_proc(void);

void test_mode(void)
{
	//show soft version.
	write_pin(LED_POWER_PIN, 0);
	//reset default value.
	eeprom_lowlevel_init();
	//run_time = 0;
	run_time = MAX_RUN_TIME;  // set to test
	store_running_time();
	ap368s.fan_level = 1;
	while(get_key_code() == KEY_MODE_CODE)IWDG->KR = 0xAAAA;	//clear	;
	write_pin(LED_POWER_PIN, 1);
	delay_ms(300);

	for(int i=0; i< VERSION_H-'0'; i++){
		delay_ms(300);
		write_pin(LED_HIGH_PIN, 0);
		delay_ms(30);
		write_pin(LED_HIGH_PIN, 1);
	}
	delay_ms(300);
	for(int i=0; i< VERSION_L-'0'; i++){
		delay_ms(300);
		write_pin(LED_CLEAN_PIN, 0);
		delay_ms(30);
		write_pin(LED_CLEAN_PIN, 1);
	} 
	delay_ms(300);

	open_hivoltage();
	start_fan(3);
	write_pin(LED_HIGH_PIN, 0);	
	write_pin(LED_MID_PIN, 0);	
	write_pin(LED_LOW_PIN, 0);			
	while(get_key_code() != KEY_POWER_CODE){
		IWDG->KR = 0xAAAA;	//clear	
	}
	while(1);	//reset. watch dog.	
}

void main_start(void)
{
	#ifdef ENABLE_WDG
	//enable WDT
	IWDG->KR = 0x5555;
	IWDG->PR = 0;
	IWDG->KR = 0x5555;
	IWDG->RLR = 300;	//timeout = 1.6ms * 300 
	IWDG->KR = 0xAAAA;	//clear
	IWDG->KR = 0xCCCC;	//start iwdg	
	#endif
	int  error = 0;
	__GPIOB_CLK_ENABLE();
	__GPIOA_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
	//init all pin
	for(U8 i=0; i< sizeof(pin_init_tab)/sizeof(pin_init_tab[0]); i++){
		write_pin(pin_init_tab[i][0],pin_init_tab[i][1]);
		config_pin(pin_init_tab[i][0],pin_init_tab[i][2]);
	}

	uart_init(&MYUART2, uart_tx2_buff,sizeof(uart_tx2_buff),uart_rx2_buff,sizeof(uart_rx2_buff));
	uart_start_rx(&MYUART2);
	HAL_ADCEx_Calibration_Start(&hadc1);
	adc_init();
	//wait power steady.
	delay_ms(300);
	sprintf((char*)temp,"version=V%c%c\r\n",VERSION_H,VERSION_L);
	uart_send_str(&MYUART2,temp);

	//init defalut status.
	eeprom_init();
	ap368s.fan_level = 1;	//others are zero.
			/* 初始化eeprom，失败就报告 */
	if(!get_running_time(&run_time) 
			&& !eeprom_lowlevel_init() 
			&& !get_running_time(&run_time) ){
		run_time = 0;
		error = 1;
		uart_send_str(&MYUART2,"eeprom error\r\n");
	}

	if(get_key_code() == KEY_MODE_CODE){
		test_mode();
	}

	if(error == 0){
		led_powerup_test();		//led indication when power up.	
	}

}  
//--------------------------------------------
void led_powerup_test(void)
{
	const U8 led_tab[]=
	{
		LED_POWER_PIN,
		LED_LOW_PIN,
		LED_MID_PIN,
		LED_HIGH_PIN,
		LED_DRYING_PIN,
		LED_CLEAN_PIN
	};

	for(U8 i=0; i< sizeof(led_tab)/sizeof(U8); i++){
		toggle_pin(led_tab[i]);
		delay_ms(80);
		toggle_pin(led_tab[i]);
	}
}

#define ENABLE_WDG
//--------------------------------------------
void my_main(void)
{
	key_srv();  // keys service function
	#ifdef ENABLE_WDG
	IWDG->KR = 0xAAAA;	//clear
	#endif
			/* if the status is poweroff, then go to sleep */
	if(!is_power_on()){
		HAL_PWR_EnterSLEEPMode(0,PWR_SLEEPENTRY_WFI);
		return;
	}

	system_srv();
	light_monitor_srv(); /* detect the environment brightness, and ajust the led brightness */
	running_time_srv();  /* clean time acc */
	//#define BLOCK_ALARM
	#ifndef BLOCK_ALARM
	error_check_srv();
	#endif
	if(is_50ms_int(DBG_TIMER)){
		//debug info
		sprintf((char*)temp,"alm=%dmv cur=%dmv li=%.3fv err_cnt=%d ",alm_volt_mv,curr_sensor_volt_mv,
				light_adc_value()*1.20/adc_aver_1v2,error_cnt);
		uart_send_str(&MYUART2,temp);
		sprintf((char*)temp,"alm-delta=%dmv cur-delta=%dmv ",alm_delta,curr_delta);
		uart_send_str(&MYUART2,temp);
		sprintf((char*)temp,"err=%d\r\n",ap368s.has_error);
		uart_send_str(&MYUART2,temp);
	}
}


#define ALARM_ADC_FILTER
//--------------------------------------------
void error_check_srv(void)
{

	static U32 alm_adc_sum,curr_adc_sum,curr_adc_temp;
	static U32 curr_adc_arr[10];
	static U32 cnt = 0;
	static U8 cyc = 0;
	const U32  error_aver_cnt = 50;	//check per 50ms
	#ifdef ALARM_ADC_FILTER
	if(ap368s.has_error == 0 && is_10ms_int(ERR_CHK_TIMER) )
	#else
	if(ap368s.has_error == 0 && is_1ms_int(ERR_CHK_TIMER) )
	#endif
	{

		//when is drying,hivoltage disabled,not check it. 
		//get current, alarm adc per 10ms.
		alm_adc_sum += alarm_adc_value();
		curr_adc_sum += current_adc_value();
		//curr_adc_arr[cnt] = current_adc_value();
		
		if(cnt%5 == 1 ){
			curr_adc_arr[cyc] = current_adc_value();
			cyc++;
			if(cyc >= 10) {
				cyc = 0;
			}
		}
		cnt++;
		if(cnt < error_aver_cnt){
			return;
		} 

		for(U8 i=0; i<10-1; i++){
			for(U8 j=0; j<10-1-i; j++){
				if(curr_adc_arr[j] > curr_adc_arr[j+1]){
					curr_adc_temp = curr_adc_arr[j+1];
					curr_adc_arr[j+1] = curr_adc_arr[j];
					curr_adc_arr[j] = curr_adc_temp;
				}
			}
		}
		alm_volt_mv = 1200*alm_adc_sum/error_aver_cnt/adc_aver_1v2;
		
		
		#ifdef ALARM_ADC_FILTER
		curr_sensor_volt_mv = 1200*(curr_adc_arr[2]+curr_adc_arr[3]+curr_adc_arr[4]+curr_adc_arr[5]+curr_adc_arr[6]+curr_adc_arr[7])/6/adc_aver_1v2;
		#else
		curr_sensor_volt_mv = 1200*curr_adc_sum/error_aver_cnt/adc_aver_1v2;
		#endif
		alm_adc_sum = 0;
		curr_adc_sum = 0;
		cnt = 0;
		cyc = 0;
		
		#ifdef ALARM_ADC_FILTER
		if(is_500ms_int(ERR_CHK_TIMER))
		#else
		if(is_50ms_int(ERR_CHK_TIMER))
		#endif
		{
			//store history adc data per 50ms
			history_alm_volt[history_wt_idx%HISTORY_ITEM_NUM] = alm_volt_mv;
			history_curr_volt[history_wt_idx%HISTORY_ITEM_NUM] = curr_sensor_volt_mv;
			history_wt_idx++;
			if(history_wt_idx >= HISTORY_ITEM_NUM*200){
				history_wt_idx = HISTORY_ITEM_NUM*100;
			}
		}
		//if power on steady, not check error 
		if(ap368s.power_up_wait_ms){
			error_cnt = 0;
			return;
		}
		if(ap368s.is_drying == 1){
			return;
		}
		
		if(curr_sensor_volt_mv >1000|| curr_sensor_volt_mv < 200){
			ap368s.has_error = 1;	
		}

		if(history_wt_idx > 60){
			//3s after powerup, and then check delta of status
			U16 idx = (HISTORY_ITEM_NUM + history_wt_idx-40)%HISTORY_ITEM_NUM;
			curr_delta = history_curr_volt[idx] -  curr_sensor_volt_mv;
			alm_delta = history_alm_volt[idx] - alm_volt_mv ;
			if(  curr_delta > 50 || curr_delta < -50){
				error_cnt++;
				if(error_cnt > 40){
					ap368s.has_error = 1;	
				}
			}else if(error_cnt){
				error_cnt --;
			}
		}else{
			error_cnt = 0;	//not ready yet
		}
		if(ap368s.has_error){
			//enter error model
			close_hivoltage();
			stop_fan();
			show_fan_led(FAN_LEVEL_OFF);
			//clear other drying mode.
			ap368s.is_drying = 0;
			ap368s.fan_dry_sec = 0;
			ap368s.fan_start_pending = 0;
		}else if(ap368s.fan_start_pending){
			//must check error and no error occured,and then start fan
			ap368s.fan_start_pending = 0;
			start_fan(ap368s.fan_level);
		}
	}
	if(ap368s.has_error && is_500ms_int(ERR_CHK_TIMER)){
		//flash fan led when error.
		if(led.low){
			led.low = 0;
			led.middle = 0;
			led.high = 0;
			led.power = 0;
			led.clean = 0;
			led.drying = 0;
		}else{
			led.low = 1;
			led.middle = 1;
			led.high = 1;
			led.power = 1;
			led.clean = 1;
			led.drying = 1;
		}
	} 
}
//-----------------------------------------------
void system_srv(void)
{
	//normal mode.
	if(is_1ms_int(SYS_TIMER)){
		static U8 cnt;
		static U32 ref_1v2_sum;
		if(ap368s.power_up_wait_ms){
			ap368s.power_up_wait_ms--;
		}
		ref_1v2_sum += ref_adc_1v2();
		cnt++;
		if(cnt == 10){
			cnt = 0;
			adc_aver_1v2 = ref_1v2_sum/10;
			ref_1v2_sum = 0;
		}
	}
	if(is_500ms_int(SYS_TIMER)){
		if(ap368s.drying_wait_ms){
			ap368s.drying_wait_ms--;
		}
		
		if(!ap368s.has_error && !ap368s.is_drying 
			&& ap368s.fan_level == FAN_LEVEL_BOOST){
			boost_mode_500ms_cnt++;

			if(boost_mode_500ms_cnt>=BOOST_KEEP_TIME_x500MS){ 	
				//end boost mode,auto to fan level 1 
				ap368s.fan_level = 1;
				boost_mode_500ms_cnt=0;
				start_fan(ap368s.fan_level);
				show_fan_led(ap368s.fan_level);	
			}
		}
		if(ap368s.fan_level != FAN_LEVEL_BOOST){
				boost_mode_500ms_cnt=0;
		}
	}
}
//----------------------------------------

//--------------------------------------------
void light_monitor_srv(void)
{
	static U8 cnt;
	static U32 sum;
	if(!is_1ms_int(LIGHT_MONITOR_TIMER)){
		return;
	}
	sum+= light_adc_value();
	if( ++cnt >= 35){
		U16 val;
		adc_aver_light = sum/35;
		val = find_led_light_level(adc_aver_light);
		//fade toward target value 
		if(val == led.light_level){
			;//do nothing
		}else if(val > led.light_level){
			//dismiss the system error.
			led.light_level ++;
		}else{
			led.light_level --;
		}

		set_led_light_strength(led.light_level);

		sum = 0;
		cnt = 0;
	}
}
//--------------------------------------------
void running_time_srv(void)
{
	//called per 0.5s.
	static U16 running_half_sec=0;
	//store method, store it when change.
	if(ap368s.is_pwr_on ==1 && ap368s.has_error ==0 && ap368s.is_drying == 0 && is_500ms_int(RUN_TIMER)){
		running_half_sec += 1;
		if(running_half_sec >= RUN_TIME_RESULATION){
			run_time++;
			running_half_sec = 0;
			store_running_time();
		}
		if(run_time >= MAX_RUN_TIME){
			enter_clean_mode();
		}
	}
}
//--------------------------------------------
void USART2_IRQHandler(void)
{
	hardware_uart_int_srv(&MYUART2);

}










