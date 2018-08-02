#ifndef	_KEY_SCAN_H_
#define _KEY_SCAN_H_
#include "mydefine.h"

#define	KEY_RELEASED_CODE	0xFFFF
#define KEY_NOT_STEADY		0x8FFF	// key status is changing, not steady yet


#define	KEY_POWER_CODE		0
#define	KEY_MODE_CODE		1
#define KEY_PWR_MODE_CODE 	2


extern U8 keyscan_interval_ms;

#define	key_scan_1ms_int()	{if(keyscan_interval_ms)keyscan_interval_ms--;} //need put this in 1ms timer interrupt 
U16 get_key_code(void);		//get current key code
U16 key_scan(void);		//if key is steady, return the code or else return KEY_NOT_STEADY


#endif
