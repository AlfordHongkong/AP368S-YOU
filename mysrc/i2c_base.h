/*
 * I2C Driver head
 * i2c.h 
 * (C) Copyright 2011 You ShanFeng
 *
 */
#ifndef _I2C_H_
#define _I2C_H_
#include "mydefine.h"
void i2c_config(int clk_pin,int data_pin, int speed_bps);
void i2c_send_stop(void);
void i2c_send_start(void);
void i2c_delay(void);
BOOL i2c_try_write(U8 val,U8 timeout);
BOOL i2c_write_8bit(U8 val);
U8 i2c_read_8bit(BOOL read_ack);

#endif

