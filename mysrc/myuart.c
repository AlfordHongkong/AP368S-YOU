#include "myboard.h"
#include "string.h"
#include "myuart.h"

extern MYUART	MYUART1;
extern MYUART	MYUART2;
extern MYUART	MYUART3;


void uart_1ms_int_srv(void)
{
	if(MYUART2.timeout &&  --MYUART2.timeout == 0){
		cirbuff_packet_split(MYUART2.rxbuff);
	}
}
//-----------------------------------------------------
BOOL uart_tx_busy(MYUART* myuart)
{
	return   0 != cirbuff_size(myuart->txbuff);
}
//------------------------------------------------------
BOOL  uart_config_speed(MYUART* myuart, U32 speed)
{
	USART_TypeDef* puart;
	if(myuart->software){
		return 0;
	}
	myuart->speed = speed;
	puart = (USART_TypeDef*) myuart->uart;
	puart->CR1 &= ~USART_CR1_UE;
	if(puart == USART1)
	{
		puart->BRR = (myuart->speed/2+HAL_RCC_GetPCLK2Freq())/myuart->speed;
	}
	else
	{
		puart->BRR =(myuart->speed/2+HAL_RCC_GetPCLK1Freq())/myuart->speed;
	}
	//enbale 
	puart->CR1 |= USART_CR1_UE;
	return 1;
}

void  uart_init(MYUART* myuart,U8* txbuff, U16 txsize, U8* rxbuff, U16 rxsize)
{
	USART_TypeDef* puart;
	if(myuart->software){
		return;
	}
	puart = (USART_TypeDef*) myuart->uart;
	cirbuff_init(myuart->rxbuff, rxbuff, rxsize);
	cirbuff_init(myuart->txbuff, txbuff, txsize);
	cirbuff_packet_init(myuart->rxbuff,0x0A, rxsize*2/3);	//stop char =0x0A by default.

	if(myuart->uart == USART1){
		//reset
		__USART1_CLK_ENABLE();
		__HAL_RCC_USART1_FORCE_RESET();
		__HAL_RCC_USART1_RELEASE_RESET();
		config_pin(PA10,  PIN_CONF_AF_INPUT|PIN_SPEED_HIGH|PIN_PULL_UP); //rx
		config_pin(PA9,  PIN_CONF_AF_PP|PIN_SPEED_HIGH|PIN_PULL_NONE); 	//tx
		HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);

	}else if(myuart->uart == USART3){
		//reset
		__USART2_CLK_ENABLE();
		__HAL_RCC_USART2_FORCE_RESET();
		__HAL_RCC_USART2_RELEASE_RESET();
		config_pin(PB11,   PIN_CONF_AF_INPUT|PIN_SPEED_HIGH|PIN_PULL_UP); //rx
		config_pin(PB10 , PIN_CONF_AF_PP|PIN_SPEED_HIGH|PIN_PULL_NONE);//tx
		HAL_NVIC_SetPriority(USART3_IRQn, 1, 0);
		HAL_NVIC_EnableIRQ(USART3_IRQn);

	}else if(myuart->uart == USART2){
		//reset
		__USART3_CLK_ENABLE();
		__HAL_RCC_USART3_FORCE_RESET();
		__HAL_RCC_USART3_RELEASE_RESET();

		config_pin(PA3,   PIN_CONF_AF_INPUT|PIN_SPEED_HIGH|PIN_PULL_UP); //rx
		config_pin(PA2,  PIN_CONF_AF_PP|PIN_SPEED_HIGH|PIN_PULL_NONE);//tx
		HAL_NVIC_SetPriority(USART2_IRQn, 1, 0);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
	}

	if(puart == USART1)
	{
		puart->BRR = (myuart->speed/2+HAL_RCC_GetPCLK2Freq())/myuart->speed;
	}
	else
	{
		puart->BRR =(myuart->speed/2+HAL_RCC_GetPCLK1Freq())/myuart->speed;
	}
	//enbale 
	puart->CR3 |= (USART_CR3_DMAT | USART_CR3_DMAR); //disable DMA.
	puart->CR1  |= (USART_CR1_UE| USART_CR1_TE |USART_CR1_RE) ;
}

//-----------------------------------------------------
void  uart_stop_rx(MYUART* myuart)
{
	USART_TypeDef* puart;
	if(myuart->software){
		return;
	}
	puart  = (USART_TypeDef*) myuart->uart;
	puart->CR1 &= ~USART_CR1_UE;
	puart->CR1 &= ~USART_CR1_RXNEIE;
	puart->CR1 |= USART_CR1_UE;

}
//----------------------------------------------------
void  uart_start_rx(MYUART* myuart)
{
	USART_TypeDef* puart;
	puart  = (USART_TypeDef*) myuart->uart;
	puart->CR1 &= ~USART_CR1_UE;
	puart->CR1 |= USART_CR1_RXNEIE;
	puart->CR1 |= USART_CR1_UE;
}
//-----------------------------------------
void uart_stop(MYUART *myuart)
{
	USART_TypeDef* puart;
	puart  = (USART_TypeDef*) myuart->uart;
	cirbuff_clear(myuart->rxbuff);
	cirbuff_clear(myuart->txbuff);
	puart->CR1&= ~(USART_CR1_UE | USART_CR1_TXEIE | USART_CR1_RXNEIE);	
}
//--------------------------------------------------------
void hardware_uart_int_srv(MYUART* myuart)
{
	USART_TypeDef* uart;
	U32  sr;
	uart = (USART_TypeDef*)myuart->uart;
	sr = uart->SR;
	if( (uart->CR1 & USART_CR1_RXNEIE) && (sr & USART_SR_RXNE) ){
		//received data.
		myuart->timeout = 3;

		if(cirbuff_packet_push(myuart->rxbuff,uart->DR)){
			myuart->timeout = 0;
		} 
	}else	if( (sr & USART_SR_TXE) && (uart->CR1 & USART_CR1_TXEIE) ){
		//send data in tx circular buff.
		U16 val = cirbuff_pop(myuart->txbuff);

		if( val < 0x100){
			uart->DR = val;
		}
		else{
			//end, no data to send.
			uart->CR1 &= ~USART_CR1_TXEIE;
		}
	}else	if(sr &USART_SR_ORE){
		//toggle_pin(PC13);
		uart->DR;
	}
}

void  uart_send_byte(MYUART* myuart, U8 byte)
{
	uart_send(myuart, &byte, 1);
}
//-----------------------------------------
void  uart_send_str(MYUART* myuart, const U8 *data)
{
	uart_send(myuart, data, strlen((char*)data));
}
//-----------------------------------------
void  uart_send(MYUART* myuart , const U8 *data, U16 size)
{
	USART_TypeDef* uart;

	if( data == NULL || size ==0){
		return;
	}
	if(myuart->software ){
		//@TODO software uart not added yet
		return;
	}

	//put data to sending circular buff
	uart = (USART_TypeDef*)(myuart->uart);
	for(U16 i=0; i< size; i++){
		while(!cirbuff_push(myuart->txbuff,data[i])){
			//buff is full, start sending and wait buff become empty
			if(!(uart->CR1 & USART_CR1_TXEIE)){
				//start sending.
				uart->CR1 |= USART_CR1_TXEIE;
			}
		}
	}

	//if not sending start sending irq.
	if(!(uart->CR1 & USART_CR1_TXEIE)){
		//start sending.
		uart->CR1 |= USART_CR1_TXEIE;
	}
}
//-----------------------------------------------------------------
U16 hex_char(U8 ch, U8* out)
{
	U8  val = ch>>4;

	//MSB
	if( val >=10){
		out[0] = val -10+ 'A';
	}
	else{
		out[0] = val + '0';
	}

	//LSB
	val = ch & 0x0F;
	if( val >=10){
		out[1] = val -10+ 'A';
	}
	else{
		out[1] = val + '0';
	}	
	return (U16)(out[0]<<8) + out[1];
}




