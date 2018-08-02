#include "i2c_base.h"
#include "mytimer.h"
#include "mygpio.h"

#include "myboard.h"

U8 temp_humi_init(void)
{
	U8 result=0;

	i2c_config(HDC1080_CLK_PIN,HDC1080_SDA_PIN,100);
	i2c_send_start();
	while(1){
		if(i2c_write_8bit(0x80)){
			break;
		}
		if(i2c_write_8bit(0x02)){
			break;
		}
		if(i2c_write_8bit(0x00)){
			break;
		}
		if(i2c_write_8bit(0x00)){
			break;
		}
		result = 1;
		break;
	}
	i2c_send_stop();
	return result;
}

S16 read_temperature(void)
{
//	temp（C）=  (VAL/65536)*165.0-40.0
	U8  ok=0;
	U16 result;

	i2c_config(HDC1080_CLK_PIN,HDC1080_SDA_PIN,100);
	i2c_send_start();
	while(1){
		if(i2c_write_8bit(0x80)){
			break;
		}
		if(i2c_write_8bit(0x00)){
			break;
		}
		ok = 1;
		break;
	}
	i2c_send_stop();
	if(ok ==0){
		return -1000;
	}
	//wait convert complete, about 10ms
	delay_ms(10);

	i2c_send_start();
	if(i2c_write_8bit(0x81)){
		i2c_send_stop();
		return -1000;
	}
	 result = i2c_read_8bit(0);
 	 result = result*256 + i2c_read_8bit(1);
 	 i2c_send_stop();

	return  (S16)(-400.0 + 1650.0*result/65536.0);
}

S16 read_humidity(void)
{
	U8  ok=0;
	U16 result;
	i2c_config(HDC1080_CLK_PIN,HDC1080_SDA_PIN,100);
	i2c_send_start();
	while(1){
		if(i2c_write_8bit(0x80)){
			break;
		}
		if(i2c_write_8bit(0x01)){
			break;
		}
		ok = 1;
		break;
	}
	i2c_send_stop();
	if(ok ==0){
		return -1000;
	}
	//wait convert complete, about 10ms
	delay_ms(10);

	i2c_send_start();
	if(i2c_write_8bit(0x81)){
		i2c_send_stop();
		return -1000;
	}
	 result = i2c_read_8bit(0);
 	 result = result*256 + i2c_read_8bit(1);
 	 i2c_send_stop();

	return  (S16)(1000.0*result/65536.0);
}


