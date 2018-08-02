#include "myboard.h"
#include "key_scan.h"

#define	KEY_SCAN_INTERVAL	10	
#define KEY_STEADY_MS		30	//a valid key must keep at least time of KEY_STEADY_MS ms. 

U8 keyscan_interval_ms;		//decrease per 1ms in timer int,scan key when it is zero.	
static U8 steady_ms;
static U16 old_code = KEY_RELEASED_CODE;


U16 key_scan(void)
{
	U16 code;
	if(keyscan_interval_ms !=0){
		return KEY_NOT_STEADY;
	}
	code= get_key_code();
	if(code == old_code){
		if(steady_ms >= KEY_STEADY_MS){
			return code;
		}
		steady_ms += KEY_SCAN_INTERVAL;
	}else{
		steady_ms = 0;
		old_code = code;
	}
	keyscan_interval_ms = KEY_SCAN_INTERVAL;
	return 	KEY_NOT_STEADY;
}

U16 get_key_code(void)
{
	if(read_pin(KEY_POWER_PIN) && read_pin(KEY_MODE_PIN)){
		return KEY_PWR_MODE_CODE;
	}
	if(read_pin(KEY_POWER_PIN)==1){
		return KEY_POWER_CODE;
	}else if(read_pin(KEY_MODE_PIN)==1){
		return KEY_MODE_CODE;
	}
	return KEY_RELEASED_CODE;
}
