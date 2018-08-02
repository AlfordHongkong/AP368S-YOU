#include "eeprom.h"
#include "i2c_base.h"
#include "myboard.h"
#define	DEV_WT_ADDR	0xA0
#define	DEV_RD_ADDR	0xA1

#define	is_page_start(addr)  ((addr & 0x0007) == 0x00)

U8 eeprom_read(U8 addr,U8* buff,U8 size)
{
	U8 i;
	i2c_config(EEPROM_CLK_PIN,EEPROM_SDA_PIN,10);
	if(!i2c_try_write(DEV_WT_ADDR, 10)){
		return 0;
	}
	i2c_write_8bit(addr);
	i2c_send_start();
	if(i2c_write_8bit(DEV_RD_ADDR)){
		i2c_send_stop();
		return 0;
	}
	for(i=0; i<size-1; i++){
		buff[i] = i2c_read_8bit(0);
	}
	buff[i] =  i2c_read_8bit(1);	//last byte
	i2c_send_stop();
	return 1;
}

U8 eeprom_write(U8 addr,U8* buff,U8 size)
{
	i2c_config(EEPROM_CLK_PIN,EEPROM_SDA_PIN,10);
	while(size){
		U8 has_send_stop = 0;
		if(!i2c_try_write(DEV_WT_ADDR,15)){
			return 0;
		}
		i2c_write_8bit(addr);
		//write page
		while(size){
			i2c_write_8bit(*buff);
			buff++;
			addr++;
			size--;
			if(is_page_start(addr)){
				has_send_stop = 1;
				i2c_send_stop();
				delay_ms(8);	// max 5ms, AT24C02A, wait page write complete.
				break;
			}
		}
		if(has_send_stop==0){
			i2c_send_stop();
			break;
		}
	}
	return 1;
}


//-------------- eeprom block function ---------------------------------
//desc: write data to different blocks to multiply the write cycles 
// --------------------------------------------------------------------- 
//
// block_data: 	[count size id checksum  [repeat user_data_0]   [repeat user_data_1]   [repeat user_data_2]]  
// eep_addr:  	  0    1    2    3         4     5 ...          		
// 		|      head data       |    block_data        |    block_data        |     block_data        |    
//
// count = how many blocks followed after the head   
// size  = bytes of each blocks  
// id	=  the current valid block from 0 to  count-1
// checksum = count + size + id;
// repeat =  write count of the block.
//
// method:   read:	read (size -1)bytes  user_data  from  block assigned by [id].
//	     write:	if repeat of current block > 250,write to next block.
//	     		or else, write to current block and add repeat count.
//	     		
//-----------------------------------------------
BOOL eeprom_block_init(U8 eep_addr,U8 count,U8 user_data_size)
{
	U8 temp[5];
	temp[0] = count;			//count
	temp[1] = user_data_size+1;		//size
	temp[2] = 0;				//id
	temp[3] = temp[0] + temp[1]+temp[2];	//checksum
	temp[4] = 0;				//repeat of first block.
	return eeprom_write(eep_addr, temp, 5);
}
//-----------------------------------------------
BOOL eeprom_block_write(U8 addr,U8* buff)
{
	U8 head[4];
	U8 repeat;
	U16 blk_addr;
	//read head.
	if(!eeprom_read(addr, head,4)){
		return 0;
	}
	if(head[0]+head[1]+head[2] != head[3] || head[2]>= head[0]
		|| head[1] <3){
		//checksum error or wrong block id.
		return 0;
	}
	//read repeat at  (addr +  4 + id*size).
	blk_addr = addr + 4 + head[2]*head[1];
	if(!eeprom_read(blk_addr, &repeat, 1)){
		return 0;
	}
	if(repeat == 0xFF){
		//switch to next block.
		repeat = 0;
		//id point to next block.
		head[2]+= 1;
		if(head[2] >= head[0]){
			head[2] = 0;	
		}
		blk_addr = addr + 4 + head[1]*head[2];
		if(!eeprom_write(blk_addr+1, buff, head[1]-1)){		//write size-1 byte user data.
			return 0;
		}
		if(!eeprom_write(blk_addr, &repeat, 1)){		//write new repeat
			return 0;
		}
		//update id
		head[3] = head[0] + head[1] + head[2];			//new checksum.
		if(!eeprom_write(addr+2, head+2, 2)){
			return 0;
		}
		return 1;
	}
	//set new write count
	repeat ++;
	if(!eeprom_write(blk_addr+1, buff, head[1]-1)){		//write size-1 byte user data.
		return 0;
	}
	return eeprom_write(blk_addr, &repeat, 1);		//write new repeat

}
//----------------------------------------------------
BOOL eeprom_block_read(U8 addr,U8* buff)
{
	U8 head[4];
	U16 user_addr;
	//read head.
	if(!eeprom_read(addr, head,4)){
		return 0;
	}
	if(head[0]+head[1]+head[2] != head[3] || head[2]>= head[0]
		|| head[1] <3){
		//checksum error or wrong block id.
		return 0;
	}
	//read repeat at  (addr +  4 + id*size).
	user_addr = addr + 5 + head[2]*head[1];
	return eeprom_read(user_addr, buff, head[1]-1);
}
