#ifndef	_AP368S_H_
#define _AP368S_H_

#include "myboard.h"

typedef struct  {
	U8 is_pwr_on:1;
	volatile U8 fan_running:1;
	U8 need_clean:1;
	U8 is_drying:1;
	U8 has_error:1;
	U8 fan_start_pending:1;		//
	U8 fan_level;			//0=off;1=low,2=middle,3=high,4=dry mode.
	U32 fan_dry_sec;		//time left to keep in drying mode.
	U16 power_up_wait_ms;
	U16 drying_wait_ms;	
}SYS_STATUS;


typedef struct {
	U16 light_level;		//light level 0-100;
	U8 power;
	U8 drying;
	U8 clean;
	U8 high;
	U8 middle;
	U8 low;
}LED_STATUS;

extern SYS_STATUS ap368s;		//global status of the project.
extern LED_STATUS led;
extern volatile U16 adc_buff[4];
extern U16 history_wt_idx;
void start_fan(U8 fan_level);
void stop_fan(void);
void led_pwm_int(void);
void init_led(void);
void deinit_led(void);

void set_power_led(U8 on);
void set_drying_led(U8 on);
void set_clean_led(U8 on);
void set_led_light_strength(U8 strength);
U16 find_led_light_level(U16 light_sensor_adc);
void show_fan_led(U8 fan_level);

void open_hivoltage(void);
void close_hivoltage(void);

void adc_init(void);
void adc_deinit(void);

void exit_clean_mode(void);
void enter_clean_mode(void);

BOOL eeprom_lowlevel_init(void);
BOOL eeprom_init(void);
BOOL store_running_time(void);
BOOL get_running_time(U16* run_time);

#define is_power_on() 		(ap368s.is_pwr_on != 0)
#define alarm_adc_value() 	(adc_buff[0])
#define light_adc_value() 	(adc_buff[1])
#define current_adc_value()	(adc_buff[2])
#define ref_adc_1v2()	  	(adc_buff[3])

#define LED_LIGHT_MAX		100		// 1k,pwm
#define FAN_PWM_CNT_MAX		330		//8Mhz,24K

//param for void start_fan(U8 fan_level)
#define	FAN_LEVEL_OFF		0
#define	FAN_LEVEL_LOW		1
#define	FAN_LEVEL_MID		2
#define	FAN_LEVEL_HIGH		3
#define	FAN_LEVEL_BOOST		4
#define	FAN_LEVEL_DRY		5

#define	DRYING_WAIT_TIME_X500MS		2*60*8		// 8·ÖÖÓ
#define	FAN_KEEP_DRY_MINUTE		30		// 0.3Hour
#define BOOST_KEEP_TIME_x500MS		(2*60*20)  	//boost mode duration, 20 minute.
#define	POWER_UP_STEADY_WAIT_MS		1200		//power up and wait 1s to check error and start fna..

//#define	RUN_TIME_TEST
#ifdef RUN_TIME_TEST
#define RUN_TIME_RESULATION		(2*10)	//1=0.5 sec,10 seconds
#else
#define RUN_TIME_RESULATION		(2*60*10)	//1=0.5 sec,10 minutines
#endif
#define MAX_RUN_TIME			(6*366)		//366Hour = 10 minutines* 6 * 366 
#define DTRING_HUMI_LIMIT		800		// level=3 humi > DTRING_HUMI_LIMIT,then enter dring mode.
#define VERSION_H			'1'
#define VERSION_L			'5'
    
#define	 EEP_TEST_ADDR			0x00
#define  EEP_FAN_LV_ADDR		0x10
#define	 EEP_RUNTIME_ADDR		0x80		
#define	 EEP_RUNTIME_BLK_SIZE		3		//total use 44byte 4+ (3+1)*10
#define	 EEP_RUNTIME_BLK_NUM		10


#endif

