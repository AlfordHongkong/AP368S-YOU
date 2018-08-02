#ifndef	_MY_BOARD_H_
#define	_MY_BOARD_H_

#include "stm32f1xx.h"
//cortex M3 bit band setting for stm32
#define SRAM_BIT_BAND_PTR(val,bit)		(volatile U32*)(0x22000000+ ((U32)&(name)-0x20000000)*32 + (bit)*4)
#define SRAM_BIT_BAND(name, bit)		(*SRAM_BIT_BAND_PTR(peripheral_name,bit))
#define	BIT_BAND_PTR(peripheral_name, bit)	(volatile U32*)(0x42000000+ ((U32)&(peripheral_name)-0x40000000)*32 + (bit)*4)
#define	BIT_BAND(peripheral_name, bit)		(*BIT_BAND_PTR(peripheral_name,bit))

#include "board_AP368S.h"

#include "mydefine.h"
#include "mygpio.h"
#include "myuart.h"
#include "mytimer.h"
#include "circular_buff.h"
#include "key_scan.h"
#include "eeprom.h"
#include "common.h"
#include "hdc1080.h"
#include "key_srv.h"
#include "ap368s.h"
#endif
