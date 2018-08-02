#ifndef	_MY_TIMER_H_
#define _MY_TIMER_H_

#include "mydefine.h"
#include "stm32f1xx.h"

#define  TIMBITS	U16

#define	KEY_SRV_TIMER		(1<<0)
#define	MONITOR_TIMER		(1<<1)
#define	DBG_TIMER		(1<<2)
#define	SYS_TIMER		(1<<3)
#define	FAN_DRY_TIMER		(1<<4)
#define	LIGHT_MONITOR_TIMER	(1<<5)
#define	ERR_CHK_TIMER		(1<<6)
#define	RUN_TIMER		(1<<7)

BOOL is_1ms_int(TIMBITS timer_mask);
BOOL is_10ms_int(TIMBITS timer_mask);
BOOL is_50ms_int(TIMBITS timer_mask);
BOOL is_500ms_sync(void);
BOOL is_500ms_int(TIMBITS timer_mask);

extern void (*timer1_int_srv)(void);
extern void (*timer2_int_srv)(void);
extern void (*timer3_int_srv)(void);
extern void (*timer4_int_srv)(void);

void init_timer1(void);
void delay_loop(U32 loop_num);
void delay_ms(U16 time_ms);
void timer1_init_pwm(unsigned freq, unsigned * buff, unsigned cnt,unsigned repeat);
void timer1_stop_pwm(void);
#endif

