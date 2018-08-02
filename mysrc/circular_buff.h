#ifndef	_CIRCULAR_BUFF_H_
#define  _CIRCULAR_BUFF_H_

#include "mydefine.h"

typedef struct 
{
	U8*	buff;
	U16	size;
	U16	write_idx;
	U16	read_idx;
	U16 	packet_idx;
	U16	packet_stop_char;
	U16	packet_max_size;
}CIRCULAR_BUFF;
 
U16 	cirbuff_packet_size(CIRCULAR_BUFF* pCir);
void cirbuff_packet_init(CIRCULAR_BUFF* pCir,U16 stop_char, U16 max_packet_size);
U16	cirbuff_packet_read(CIRCULAR_BUFF* pCir, U8* buff, U16 max_size);
void 	cirbuff_packet_split(CIRCULAR_BUFF* pCir);
BOOL cirbuff_packet_push(CIRCULAR_BUFF* pCir,U8 val);

void cirbuff_clear( CIRCULAR_BUFF* p_cirbuff );
U16 cirbuff_size( CIRCULAR_BUFF* p_cirbuff );			// bytes number stored in buff
U8 cirbuff_value( CIRCULAR_BUFF* p_cirbuff, U16 index );	// char of stored content.	
U16 cirbuff_space( CIRCULAR_BUFF* p_cirbuff );			// space remaiend for load new coming byte.

U16 cirbuff_pop( CIRCULAR_BUFF* p_cirbuff );			// returned LSB = fetch byte, HSB=0 mean ok. HSB =1 mean empty.
BOOL cirbuff_push(CIRCULAR_BUFF* p_cirbuff, U8 val);
// push data any way, if it is full, discard oldest byte
void cirbuff_push_ex(CIRCULAR_BUFF* p_cirbuff, U8 val);				
void cirbuff_init(CIRCULAR_BUFF* p_cirbuff, U8* buff, U16 size);
#endif

