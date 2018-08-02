/*
 * I2C  Driver
 * (C) Copyright 2011 You ShanFeng
 *
 * History:
 *
 * v1.0 2011/12/26 initial  release
 */
#include "mydefine.h"
#include "i2c_base.h"
#include "mytimer.h"
#include "mygpio.h"

static U8 pin_i2c_clk;
static U8 pin_i2c_data;
static U32 i2c_delay_cnt;

void i2c_config(int clk_pin,int data_pin, int speed_bps)
{
	pin_i2c_clk=clk_pin;
	pin_i2c_data=data_pin;
	//              1
	// T_dly     -----------
	//           speed_bps *2 
	//------- = ----------------------------
	//            1
	//T_loop    -------- *  DLY_LOOP_CLK_NUM
	//           pba_hz
	i2c_delay_cnt=10;		//about 50kbps at 8Mhz
	set_pin(pin_i2c_data);
	set_pin(pin_i2c_clk);
}

void i2c_delay(void)
{
	delay_loop(i2c_delay_cnt);
}

void i2c_data_output(void)
{
	//config_pin(pin_i2c_data,GPIO_OUT);
}
void i2c_data_input(void)
{
	set_pin(pin_i2c_data);
	//config_pin(pin_i2c_data,GPIO_INPUT_FLOATING);
}
BOOL i2c_data_get(void)
{
	return read_pin(pin_i2c_data);
}
BOOL i2c_clk_get(void)
{
	return read_pin(pin_i2c_clk);
}
//-----------------------------------------------------------
/*-------------------------------------------------------
*/
void i2c_clk_heigh(void)
{
	set_pin(pin_i2c_clk);
}
void i2c_clk_low(void)
{
	clr_pin(pin_i2c_clk);
}	
void i2c_data_heigh(void)
{
	set_pin(pin_i2c_data);
}
void i2c_data_low(void)
{
	clr_pin(pin_i2c_data);
}

void i2c_send_stop(void)
{
	i2c_data_low();	
	i2c_delay();
	i2c_clk_heigh();
	i2c_delay();
	i2c_data_heigh();
	i2c_delay();
}
/*-------------------------------------------------------
*/
void i2c_send_start(void)
{
	i2c_data_heigh();	
	i2c_delay();
	i2c_clk_heigh();
	i2c_delay();
	i2c_data_low();	
	i2c_delay();
	i2c_clk_low();
	i2c_delay();
}

BOOL i2c_try_write(U8 val,U8 timeout)
{
	while(1){
		i2c_send_start();
		if(!i2c_write_8bit(val)){
			//ok, ack = 0;
			return 1;
		}
		if(timeout == 0){
			//failed
			break;
		}
		delay_ms(2);
		if(timeout > 2)
			timeout -= 2;
		else{
			timeout = 0;
		}
	}
	i2c_send_stop();
	return 0;	//failed
}
/*-------------------------------------------------------
*/
BOOL i2c_write_8bit(U8 val)
{
	unsigned char	i,write_ack;
	for(i=8;i>0;i--){
		if(val & 0x80)		
			i2c_data_heigh();
		else
			i2c_data_low();
		val<<=1;
		i2c_delay();
		i2c_clk_heigh();
		i2c_delay();	
		i2c_clk_low();
	}
	i2c_data_input();
	i2c_delay();
	i2c_clk_heigh();
	i2c_delay();
	write_ack=i2c_data_get();
	i2c_clk_low();

	i2c_data_output();
	return write_ack;
}

/*-------------------------------------------------------
 * if read_ack=1  i2c_data ouput heigh at the ninth clock
 * if read_ack=0  i2c_data ouput low at the ninth colck
 */
U8 i2c_read_8bit(BOOL read_ack)
{
	unsigned char	i; 
	/*return value*/	
	unsigned char   retval=0;	

	i2c_data_input();		
	for(i=8;i>0;i--){
		i2c_delay();
		retval<<=1;
		i2c_clk_heigh();
		i2c_delay();
		retval&=~0x01u;
		if(i2c_data_get())
			retval|=0x01u;
		i2c_clk_low();
	}
	/* set read ACK*/
	i2c_data_output();	 
	if(read_ack){				
		i2c_data_heigh();
	}		
	else{
		i2c_data_low();
	}		
	i2c_delay();
	i2c_clk_heigh();
	i2c_delay();
	i2c_clk_low();
	return retval;
}
