#include "myboard.h"
#include "AP368S.h"

U16  cur_key_code;
extern U32 boost_mode_500ms_cnt;
static U16 key_hold_time_ms;
static BOOL disable_key_mode;		
//-----------------------------------
void key_srv_1ms_int(void)
{
	if( cur_key_code != KEY_RELEASED_CODE){
		key_hold_time_ms ++;
	}
}
//-----------------------------------
void key_srv(void)
{
	U16  code;
	code =  key_scan();
	if(code == KEY_NOT_STEADY){
		//key not steady.
		return;
	}

	if(code == cur_key_code){
		//key repeat 
		if( code != KEY_RELEASED_CODE){
			key_repeat(code);
		}
	}else if( code == KEY_RELEASED_CODE){
		//key release 
		key_release(cur_key_code);
		cur_key_code = KEY_RELEASED_CODE;
		return; 
	}else{
		cur_key_code = code;
		key_down(cur_key_code);
	}
}
//-----------------------------------
void key_down(U16 key_code)
{
	if( key_code == KEY_POWER_CODE){
		key_power_down();
	}else if( key_code == KEY_MODE_CODE){
		key_mode_down();
	}else if( key_code == KEY_PWR_MODE_CODE){
		key_pwr_mode_down();
	}
} 
//-----------------------------------
void key_repeat(U16 code)
{
	if(disable_key_mode == 0 && code == KEY_MODE_CODE){
		key_mode_repeat();
	}
}
//-----------------------------------
void key_release(U16 code)
{
	if(disable_key_mode == 0 && code == KEY_MODE_CODE){
		key_mode_release();
	}
	disable_key_mode = 0;
	key_hold_time_ms = 0;
}
//-----------------------------------
void key_mode_repeat(void)
{
	if(disable_key_mode){
		return;
	}

	if(is_power_on() && ap368s.need_clean && key_hold_time_ms > 3000 ){
		//hold to clear clean indication
		exit_clean_mode();
		disable_key_mode = 1;	//keep no effect till all key released.
	}	
}
//-----------------------------------
void key_mode_release(void)
{
	if(disable_key_mode){
		return;
	}
	if(ap368s.has_error == 0 && disable_key_mode ==0 && is_power_on() && ap368s.is_drying == 0){
		//not on drying mode.
		fan_level_switch();
	}
}
//-----------------------------------
void key_power_down(void)
{
	if( ap368s.is_pwr_on){
		power_off();
	}else{
		power_on();
	}
}
//-----------------------------------
void key_mode_down(void)
{
	if(ap368s.has_error){
		//error mode, this key disabled;
		disable_key_mode = 1;
		return;
	}
}
//-----------------------------------
void key_pwr_mode_down(void)
{


}
//-----------------------------------
void fan_level_switch(void)
{
	ap368s.fan_level +=1;
	if(ap368s.fan_level > 4){
		ap368s.fan_level = 1;
	}
	start_fan(ap368s.fan_level);
	show_fan_led(ap368s.fan_level);
	boost_mode_500ms_cnt=0;
}
//-----------------------------------
void power_on(void)
{
	extern U16 power_up_wait_ms;
	ap368s.is_pwr_on = 1;

	init_led();
	open_hivoltage();
	ap368s.fan_start_pending = 1;	//starts fan after hivoltage steady and no error 
	ap368s.fan_dry_sec = 0;
	ap368s.drying_wait_ms = DRYING_WAIT_TIME_X500MS;
	delay_ms(5);	//wait adc value updated in timer int.

	led.power = 1;	//open power led.
	led.light_level = find_led_light_level(light_adc_value());
	set_led_light_strength(led.light_level);
	show_fan_led(ap368s.fan_level);
	ap368s.power_up_wait_ms = POWER_UP_STEADY_WAIT_MS;	//wait steady for afterward error check.
	history_wt_idx = 0;
}
//-----------------------------------
void power_off(void)
{
	ap368s.is_pwr_on = 0;
	ap368s.has_error = 0;	//clear error status.
	ap368s.is_drying = 0;  //clear drying status.
	ap368s.fan_dry_sec = 0;
	ap368s.fan_start_pending = 0;
	ap368s.need_clean = 0;
	
	boost_mode_500ms_cnt=0;
	close_hivoltage();
	stop_fan();
	deinit_led();
	ap368s.power_up_wait_ms = 0;
	//stop_running_timer();
}



