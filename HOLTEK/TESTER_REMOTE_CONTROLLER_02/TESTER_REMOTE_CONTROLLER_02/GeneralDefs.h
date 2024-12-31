// ==================================================================================================================================================
// Revision History:
// Version 		01.0		93/07/17	Nejatbakhsh.
// ==================================================================================================================================================
// Note: This file should be included in all projects as an standard file.
// ==================================================================================================================================================
#ifndef __GENERALDEFS_H__
#define __GENERALDEFS_H__
// ==================================================================================================================================================
// C standard types redefinitions:
// ==================================================================================================================================================
typedef unsigned char		uint8 ;
typedef   signed char		 int8 ;
typedef unsigned int		uint16 ;
typedef   signed int		 int16;
typedef unsigned long		uint32;
typedef   signed long		 int32;

// ==================================================================================================================================================
// Program general definitions:
// ==================================================================================================================================================
#define TRUE									1
#define FALSE									0
#define HIGH									1
#define LOW										0
#define ON										1
#define OFF										0
#define ENABLE									1
#define DISABLE									0
#define ENABLED									1
#define DISABLED								0
#define PIN_DIR_INPUT							1 // This may vary among MCUs.
#define PIN_DIR_OUTPUT							0 // This may vary among MCUs.

#define BIT0									(uint8)(0x01)
#define BIT1									(uint8)(0x02)
#define BIT2									(uint8)(0x04)
#define BIT3									(uint8)(0x08)
#define BIT4									(uint8)(0x10)
#define BIT5									(uint8)(0x20)
#define BIT6									(uint8)(0x40)
#define BIT7									(uint8)(0x80)

// 7-segment digit patterns:
#define	D7S_OFF									(uint8)(0x00)
#define	D7S_0									(uint8)(0x3f)
#define	D7S_1									(uint8)(0x06)
#define	D7S_2									(uint8)(0x5b)
#define	D7S_3									(uint8)(0x4f)
#define	D7S_4									(uint8)(0x66)
#define	D7S_5									(uint8)(0x6d)
#define	D7S_6									(uint8)(0x7d)
#define	D7S_7									(uint8)(0x07)
#define	D7S_8									(uint8)(0x7f)
#define	D7S_9									(uint8)(0x6F)

#define	D7S_A									(uint8)(0x77)
#define	D7S_b									(uint8)(0x7C)
#define	D7S_C									(uint8)(0x39)
#define	D7S_d									(uint8)(0x5E)
#define	D7S_E									(uint8)(0x79)
#define	D7S_F									(uint8)(0x71)
#define	D7S_G									(uint8)(0xFF) // Not usable!
#define	D7S_H									(uint8)(0x76)
#define	D7S_I									(uint8)(0xFF) // Not usable!
#define	D7S_J									(uint8)(0x1E)
#define	D7S_K									(uint8)(0xFF) // Not usable!
#define	D7S_L									(uint8)(0x38)
#define	D7S_M									(uint8)(0xFF) // Not usable!
#define	D7S_n									(uint8)(0x54)
#define	D7S_o									(uint8)(0x5C)
#define	D7S_P									(uint8)(0x73)
#define	D7S_q									(uint8)(0x67)
#define	D7S_r									(uint8)(0x50)
#define D7S_S		`							(uint8)(0x6d)
#define D7S_T									(uint8)(0x31)
#define	D7S_U									(uint8)(0x3e)
#define	D7S_v									(uint8)(0x1C)
#define	D7S_W									(uint8)(0xFF) // Not usable!
#define	D7S_X									(uint8)(0xFF) // Not usable!
#define D7S_y									(uint8)(0x6E)
#define D7S_Z									(uint8)(0xFF) // Not usable!
// ==================================================================================================================================================
// Common user types:
// ==================================================================================================================================================
// Use this union for all of your program control flags.
union Bitfield
{
	uint8 reg;
	struct
	{
		uint8 	bit0 	:1;
		uint8 	bit1 	:1;
		uint8 	bit2 	:1;
		uint8 	bit3 	:1;
		uint8 	bit4 	:1;
		uint8 	bit5 	:1;
		uint8 	bit6 	:1;
		uint8 	bit7 	:1;
	};
};
typedef union Bitfield BitField;

// ==================================================================================================================================================
// End Of File
// ==================================================================================================================================================
#endif // __GENERALDEFS_H__