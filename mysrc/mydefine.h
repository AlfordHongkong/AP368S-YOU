#ifndef	_MY_DEIFNE_H_
#define _MY_DEFINE_H_

#include <stdint.h>

#define		Little_Endian	0
#define		Big_Endian		1

#define		MY_ENDIAN	Little_Endian

//----------------- data type -------------------
#define		BOOL				uint8_t
#define		U32				uint32_t
#define		S32				int32_t
#define		U16				uint16_t
#define		S16				int16_t
#define		U8				uint8_t
#define		S8				int8_t



#if MY_ENDIAN == Little_Endian
// little endian , start with LSB byte, MSB3 is heighest byte.
//MSB3 mean heighest byte.
//----------------------------------------
#define MSB0(val)   (*((U8*)(&(val))+0))	
#define MSB1(val)   (*((U8*)(&(val))+1))
#define MSB2(val)   (*((U8*)(&(val))+2))
#define MSB3(val)   (*((U8*)(&(val))+3))

#define MSB(val)    MSB1(val)
#define LSB(val)    MSB0(val)
//----------------------------------------
#else
#if MY_ENDIAN == Big_Endian
//big endian  start with MSB byte 
//MSB3 mean heighest byte.
#define MSB3(val)   (*((U8*)(&(val))+0))
#define MSB2(val)   (*((U8*)(&(val))+1))
#define MSB1(val)   (*((U8*)(&(val))+2))
#define MSB0(val)   (*((U8*)(&(val))+3))	

#define MSB(val)    MSB3(val)
#define LSB(val)    MSB2(val)
//----------------------------------------
#endif
#endif

#ifndef	ABS
#define		ABS(a,b)	(((a) > (b))? (a)-(b):(b)-(a))
#endif
#ifndef MAX
#define		MAX(a,b)	(((a) > (b))? (a):(b))
#endif
#ifndef MIN
#define		MIN(a,b)	(((a) < (b))? (a):(b))
#endif

//////////////////////////////////////////////////////
#ifndef		TRUE
#define		TRUE						1
#endif

#ifndef		FALSE
#define		FALSE						0
#endif

#ifndef		YES
#define		YES						1
#endif

#ifndef		Bit0
#define		Bit0						0x01
#endif
#ifndef		Bit1
#define		Bit1						0x02
#endif
#ifndef		Bit2
#define		Bit2						0x04
#endif
#ifndef		Bit3
#define		Bit3						0x08
#endif
#ifndef		Bit4
#define		Bit4						0x10
#endif
#ifndef		Bit5
#define		Bit5						0x20
#endif
#ifndef		Bit6
#define		Bit6						0x40
#endif
#ifndef		Bit7
#define		Bit7						0x80
#endif

#ifndef		Bit8
#define		Bit8						0x0100
#endif
#ifndef		Bit9
#define		Bit9						0x0200
#endif
#ifndef		Bit10
#define		Bit10						0x0400
#endif
#ifndef		Bit11
#define		Bit11						0x0800
#endif
#ifndef		Bit12
#define		Bit12						0x1000
#endif
#ifndef		Bit13
#define		Bit13						0x2000
#endif
#ifndef		Bit14
#define		Bit14						0x4000
#endif
#ifndef		Bit15
#define		Bit15						0x8000
#endif

#ifndef		D0
#define		D0							0x01
#endif
#ifndef		D1
#define		D1							0x02
#endif
#ifndef		D2
#define		D2							0x04
#endif
#ifndef		D3
#define		D3							0x08
#endif
#ifndef		D4
#define		D4							0x10
#endif
#ifndef		D5
#define		D5							0x20
#endif
#ifndef		D6
#define		D6							0x40
#endif
#ifndef		D7
#define		D7							0x80
#endif

#ifndef		D8
#define		D8							(1<<8)
#endif
#ifndef		D9
#define		D9							(1<<9)
#endif
#ifndef		D10
#define		D10							(1<<10)
#endif
#ifndef		D11
#define		D11							(1<<11)
#endif
#ifndef		D12
#define		D12							(1<<12)
#endif
#ifndef		D13
#define		D13							(1<<13)
#endif
#ifndef		D14
#define		D14							(1<<14)
#endif
#ifndef		D15
#define		D15							(1<<15)
#endif



#endif
