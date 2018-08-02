#include "mydefine.h"
#include "common.h"
//find value according input table.

U16  get_table_value(U16 input_key,const U16* table)
{
	//table example
	// U16 tab[]={
	//    //key, value
	//    100, 10,
	//    200, 5,
	//    0,   0,
	//	};
	U16 key,val;
	key = *table++;
	val = *table++;
	while(input_key >= key && *table >= key ){ 
		//get next item.
		key = *table++;
		val = *table++;
	}
	return val;
}
