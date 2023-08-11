/* 
 * File:   LCD8bits.h
 * Author: Black
 *
 * Created on 25 de julio de 2023, 12:16 PM
 */

#ifndef LCD8BITS_H
#define	LCD8BITS_H

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000
#endif

#ifndef RS
#define RS PORTEbits.RE0
#endif

#ifndef EN
#define EN PORTEbits.RE1
#endif

#ifndef D4
#define D4 PORTDbits.RD4
#endif

#ifndef D5
#define D5 PORTDbits.RD5
#endif

#ifndef D6
#define D6 PORTDbits.RD6
#endif

#ifndef D7
#define D7 PORTDbits.RD7
#endif

#include <xc.h> // include processor files - each processor file is guarded.  

//LCD Functions Developed by electroSome


void Lcd_Port8(char a);

void Lcd_Cmd8(char a);

void Lcd_Clear8(void);

void Lcd_Set_Cursor8(char a, char b);

void Lcd_Init8(void);

void Lcd_Write_Char8(char a);

void Lcd_Write_String8(char *a);

void Lcd_Shift_Right8(void);

void Lcd_Shift_Left8(void);

#endif	/* LCD8BITS_H */

