/* Archivo: LCD8bits.c
 * Dispositivo: PIC16F887
 * Autor: Kevin Alarcón
 * Compilador: XC8(v2.40), MPLABX V6.05
 * 
 * 
 * Programa: Librería de configuracion para la LCD en 8bits
 * Hardware: NA
 * 
 * Creado: 20 de julio, 2023
 * Última modificación: 28 de julio, 2023
 */

#include "LCD8bits.h"

void Lcd_Port8(char a) {
    PORTD = a;
}

void Lcd_Cmd8(char a) {
    RS = 0; // => RS = 0 // Dato en el puerto lo va interpretar como comando
    Lcd_Port8(a);
    EN = 1; // => E = 1
    __delay_ms(4);
    EN = 0; // => E = 0
}

void Lcd_Clear8(void) {
    Lcd_Cmd8(0);
    Lcd_Cmd8(1);
}

void Lcd_Set_Cursor8(char a, char b) {
    char temp, z, y;
    if (a == 1) {
        temp = 0x80 + b - 1;
        Lcd_Cmd8(temp);
    } else if (a == 2) {
        temp = 0xC0 + b - 1;
        Lcd_Cmd8(temp);
    }
}

void Lcd_Init8(void) {
    Lcd_Port8(0x00);
    __delay_ms(20);
    Lcd_Cmd8(0x30);
    __delay_ms(5);
    Lcd_Cmd8(0x30);
    __delay_ms(11);
    Lcd_Cmd8(0x30);
    /////////////////////////////////////////////////////
    Lcd_Cmd8(0x3C);
    //Lcd_Cmd(0x02);
    //Lcd_Cmd(0x08);
    //Lcd_Cmd(0x00);
    Lcd_Cmd8(0x0C);
    Lcd_Cmd8(0x01);
    //Lcd_Cmd(0x00);
    Lcd_Cmd8(0x06);
}

void Lcd_Write_Char8(char a) {
    char temp, y;
    temp = a; //& 0x0F;
    y = a; //& 0xF0;
    RS = 1; // => RS = 1
    Lcd_Port8(y); //Data transfer
    EN = 1;
    __delay_us(40);
    EN = 0;
   
}

void Lcd_Write_String8(char *a) {
    int i;
    for (i = 0; a[i] != '\0'; i++)
        Lcd_Write_Char8(a[i]);
}

void Lcd_Shift_Right8(void) {
    Lcd_Cmd8(0x01);
    Lcd_Cmd8(0x0C);
}

void Lcd_Shift_Left8(void) {
    Lcd_Cmd8(0x01);
    Lcd_Cmd8(0x08);
}