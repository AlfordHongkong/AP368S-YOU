#ifndef	BOARD_AP368S_H__
#define	BOARD_AP368S_H__
//hardware configuration of AP368S
#include "mygpio.h"



#define	 KEY_POWER_PIN	PB5
#define	 KEY_MODE_PIN	PB3

#define	 LED_CLEAN_PIN	PB14
#define	 LED_DRYING_PIN	PB13
#define	 LED_POWER_PIN	PB15
#define	 LED_HIGH_PIN	PB12
#define	 LED_MID_PIN	PB8
#define	 LED_LOW_PIN	PB1

#define	 HDC1080_CLK_PIN	PA12
#define	 HDC1080_SDA_PIN	PA11
#define	 EEPROM_CLK_PIN		PB6
#define	 EEPROM_SDA_PIN		PB7

#define	 FAN_OPEN_PIN		PB9	//timer4 ch4 pwm
#define	 HIVOLT_EBL_PIN		PA8

#define	 ALARM_ADC_CHANNEL	1	//adc1
#define	 CURRENT_ADC_CHANNEL	6	//adc6
#define  LIGHT_ADC_CHANNEL	4	//adc4

#define  ALARM_ADC_PIN		PA1
#define  CURRENT_ADC_PIN	PA6
#define  LIGHT_ADC_PIN		PA4
#endif


