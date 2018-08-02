#include "circular_buff.h"
// cirbuff_packet_xxx  function. store data in packets.
//usage: init:		cirbuff_packet_init(cir);
//		   push:		cirbuff_push(xx); cirbuff_packet_split(); 
//		   pop			cirbuff_packet_read();
void cirbuff_packet_init(CIRCULAR_BUFF* pCir,U16 stop_char, U16 max_packet_size)
{
	if(!pCir){
		return;
	}
	pCir->read_idx = 0;
	pCir->write_idx = 2;
	pCir->packet_idx = 0;
	pCir->buff[0] = 0;
	pCir->buff[1] = 0;

	pCir->packet_stop_char = stop_char;
	pCir->packet_max_size = max_packet_size;
}

//-------------------------------------------------------
U16	  cirbuff_packet_size(CIRCULAR_BUFF* pCir)
{
	return cirbuff_value(pCir,0)*256 + cirbuff_value(pCir,1);
}
//------------------------------------------------------------------
U16	  cirbuff_packet_read(CIRCULAR_BUFF* pCir, U8* buff, U16 max_size)
{
	U16 size;
	//NOTE: must keep read_idx point to packet start.
	if(pCir == 0){
		return 0;
	}

	size = cirbuff_packet_size(pCir);
	if(size == 0){
		//mean no packet ready.
		return 0;
	}
	//skip size

	cirbuff_pop(pCir);
	cirbuff_pop(pCir);
	//load data
	for(U16 i=0; i< size; i++){
		U8 ch;
		ch = (U8)cirbuff_pop(pCir);
		if(i < max_size){
			buff[i] = ch;
		}
	}
	
	return size;
}

BOOL cirbuff_packet_push(CIRCULAR_BUFF* pCir,U8 val)
{
	if( 0 == cirbuff_push(pCir,val)){
		return 0;
	}		
	if((U16)val == pCir->packet_stop_char){
		cirbuff_packet_split(pCir);
		return 1;
	}
	if(pCir->packet_max_size <= cirbuff_value(pCir,0)*256 + cirbuff_value(pCir,1)){
		cirbuff_packet_split(pCir);
		return 1;
	}
	return 0;
}
//---------------------------------------------------
void cirbuff_packet_split(CIRCULAR_BUFF* pCir)
{
	U16 index;
	U16 size;

	//make sure space >=3
	if( pCir->write_idx >= pCir->read_idx){
		if(pCir->size + pCir->read_idx - pCir->write_idx < 3){
			return;
		}
	}else if(pCir->read_idx - pCir->write_idx < 3){
		return;
	}
	//get packet size
	if(pCir->write_idx >= pCir->packet_idx){
		size  = pCir->write_idx - pCir->packet_idx;
	}
	else{
		size = pCir->write_idx + pCir->size - pCir->packet_idx; 
	}
	if(size <=2){
		return;
	}
	//store size at begin of packet.
	size -=2;	//exclude the length byte.
	index = pCir->packet_idx;
	pCir->buff[index++]= (size>>8);
	if( index >= pCir->size){
		index = 0;
	}	
	pCir->buff[index]= (U8)(size);

	//prepare next packet.
	index = pCir->packet_idx = pCir->write_idx;
	pCir->buff[index++]= 0;
	if( index >= pCir->size){
		index = 0;
	}
	pCir->buff[index++]= 0;
	if( index >= pCir->size){
		index = 0;
	}
	pCir->write_idx = index;
}


void cirbuff_init(CIRCULAR_BUFF* pCir, U8* buff, U16 size)
{
	if(!pCir){
		return;
	}
	pCir->read_idx = 0;
	pCir->write_idx = 0;
	pCir->buff = buff;
	pCir->size = size;
	pCir->packet_idx=0;
	if(buff == 0 || size ==0){
		pCir->buff = 0;
		pCir->size = 0;	
	}
}

void cirbuff_clear(CIRCULAR_BUFF* pCir)
{
	if(!pCir){
		return;
	}
	pCir->write_idx  = pCir->read_idx;

}	
//-----------------------------
U16 cirbuff_size(CIRCULAR_BUFF* pCir)
{
	if(!pCir){
		return 0;
	}
	if( pCir->write_idx >= pCir->read_idx){ 
		return  pCir->write_idx - pCir->read_idx;
	}
	return  pCir->size + pCir->write_idx - pCir->read_idx;
}
//--------------------------------------
U8 cirbuff_value(CIRCULAR_BUFF* pCir, U16 index)
{
	U16 new_idx;
	if(!pCir){
		return 0;
	}
	new_idx = pCir->read_idx + index;
	if( new_idx >= pCir->size){
		new_idx -= pCir->size;
	}
	return pCir->buff[new_idx];
}
//--------------------------------------
U16 cirbuff_pop(CIRCULAR_BUFF* pCir)
{
	U16 retval=0x100;		// value > 0xFF mean is empty. 
	if(!pCir){   
		return retval;
	}
	if(pCir->read_idx != pCir->write_idx){
		retval = pCir->buff[pCir->read_idx];
		pCir->read_idx++;
		if(pCir->read_idx >= pCir->size){
			pCir->read_idx = 0;
		}
	}
	return retval;
}
// if is full, pop out one byte, discard it,
// and then push the data.
void cirbuff_push_ex(CIRCULAR_BUFF* pCir, U8 val)
{
	U16 new_idx;
	if(!pCir || pCir->size ==0){
		return;
	}
	new_idx = pCir->write_idx+1;
	if( new_idx >= pCir->size){
		new_idx = 0;
	}
	if(new_idx == pCir->read_idx){
		//is full, pop out one byte
		pCir->read_idx++;
		if(pCir->read_idx >= pCir->size){
			pCir->read_idx = 0;
		}		
	}
	pCir->buff[pCir->write_idx]= val;
	pCir->write_idx = new_idx;	

}
//--------------------------------------
BOOL cirbuff_push(CIRCULAR_BUFF* pCir, U8 val)
{
	if(!pCir){
		return 0;
	}
	U16 new_idx;

	new_idx = pCir->write_idx+1;
	if( new_idx >= pCir->size){
		new_idx = 0;
	}
	if(new_idx == pCir->read_idx){
		//is full
		return 0;
	}
	pCir->buff[pCir->write_idx]= val;
	pCir->write_idx = new_idx;

	return 1;
}
//-----------------------------
U16 cirbuff_space(CIRCULAR_BUFF* pCir)
{
	if(!pCir){
		return 0;
	}
	if( pCir->write_idx >= pCir->read_idx){
		return  pCir->size + pCir->read_idx - pCir->write_idx;
	}
	return   pCir->read_idx - pCir->write_idx;
}

