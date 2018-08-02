#ifndef	_HDC1080_H
#define _HDC1080_H
#include "mydefine.h"
U8 temp_humi_init(void);
S16 read_temperature(void);	//temperature*100
S16 read_humidity(void);	//humi*10000

#endif
