#ifndef	_EEPROM_H_
#define _EEPROM_H_

#include "mydefine.h"

U8 eeprom_read(U8 addr,U8* buff,U8 size);
U8 eeprom_write(U8 addr,U8* buff,U8 size);


BOOL eeprom_block_read(U8 addr,U8* buff);
BOOL eeprom_block_write(U8 addr,U8* buff);
BOOL eeprom_block_init(U8 eep_addr,U8 count,U8 user_data_size);

#endif
