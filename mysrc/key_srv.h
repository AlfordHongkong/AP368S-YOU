#ifndef	_KEY_SRV_H_
#define KEY_SRV_H_
#include "mydefine.h"

//key srv base function
void key_srv(void);
void key_srv_1ms_int(void);
void key_down(U16 key_code);
void key_release(U16 code);
void key_repeat(U16 code);

//key process function.
void key_power_down(void);
void key_pwr_mode_down(void);
void fan_level_switch(void);
void power_on(void);
void power_off(void);

void key_mode_down(void);
void key_mode_release(void);
void key_mode_repeat(void);
#endif
