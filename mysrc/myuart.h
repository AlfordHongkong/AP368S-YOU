#ifndef	_MY_UART_H
#define	_MY_UART_H
#include "stm32f1xx.h"
#include "mydefine.h"
#include "circular_buff.h"

typedef  struct{
	void*	uart;								//point to hardware register map or software uart struct	
	U32	speed;
	//is virtual software or hardware uart.
	CIRCULAR_BUFF*	rxbuff;
	CIRCULAR_BUFF*	txbuff;
	BOOL  	software;	
	U32	timeout;
	U32	packet_interval;	//time to divid bytes to packet.
}MYUART;

void uart_1ms_int_srv(void);

void  uart_init(MYUART* myuart, U8* txbuff, U16 txsize, U8* rxbuff, U16 rxsize);

void  uart_stop_rx(MYUART* myuart);
void  uart_start_rx(MYUART* myuart);
void  uart_stop(MYUART *myuart);
BOOL  uart_tx_busy(MYUART* myuart);
BOOL  uart_config_speed(MYUART* myuart, U32 speed);

void  uart_send_byte(MYUART* myuart, U8 byte);
void  uart_send(MYUART* myuart, const U8* data, U16 Size);
void  uart_send_str(MYUART* myuart, const U8* data);
void  hardware_uart_int_srv(MYUART* myuart);

U16 hex_char(U8 ch, U8* out);



#endif
