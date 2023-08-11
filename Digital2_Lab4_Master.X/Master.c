/* Archivo: Master.c
 * Dispositivo: PIC16F887
 * Autor: Kevin Alarcón
 * Compilador: XC8(v2.40), MPLABX V6.05
 * 
 * 
 * Programa: Realizar un reloj que permita modificar su hora y fecha exacta
 * Hardware: Botones, reloj RTC y una LCD
 * 
 * Creado: 08 de agosto, 2023
 * Última modificación: 11 de agosto, 2023
 */
//*****************************************************************************
// Palabra de configuración
//*****************************************************************************
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//*****************************************************************************
// Definición e importación de librerías
//*****************************************************************************
#include <stdint.h>
#include <pic16f887.h>
#include "I2C.h"
#include "LCD8bits.h"
#include <xc.h>
#include <stdio.h>
//*****************************************************************************
// Definición de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000
#define RS RE0
#define EN RE1
#define D4 RD4
#define D5 RB5
#define D6 RD6
#define D7 RD7
unsigned char ds3231_direccion = 0x68;
unsigned char ds3231_escribir = 0;
unsigned char ds3231_leer = 0;
unsigned char ds3231_direccion_segundos = 0x00;
unsigned char ds3231_direccion_minutos = 0x01;
unsigned char ds3231_direccion_horas = 0x02;
unsigned char ds3231_direccion_dias = 0x04;
unsigned char ds3231_direccion_meses = 0x005;
unsigned char ds3231_direccion_biciestos = 0x06;

//*****************************************************************************
// Definición de funciones para que se puedan colocar después del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);
uint8_t bcd_a_decimal(uint8_t bcd);
unsigned int int_a_bcd(unsigned int value);
unsigned char convertir_a_bcd(unsigned char dato);
void escribir_reloj(unsigned char direccion, unsigned int valor);
unsigned int leer_reloj(unsigned char direccion);

uint8_t segundos_decimal = 0;
uint8_t minutos_decimal = 0;
uint8_t horas_decimal = 0;
uint8_t dia_decimal = 0;
uint8_t meses_decimal = 0;
uint8_t biciesto_decimal = 0; //año
uint8_t contador = 0;
uint8_t ADC = 0;
uint8_t minutos = 0;
uint8_t horas = 0;
uint8_t dias = 0;
uint8_t meses = 0;
uint8_t biciestos = 0; //años
char CadADC[5];
char Segundo[5];
char Minuto[5];
char Hora[5];
char Dia[5];
char Mes[5];
char Biciesto[5]; //Año
//*****************************************************************************
// Código de Interrupción 
//*****************************************************************************
void __interrupt() isr(void){
    if(INTCONbits.RBIF){
        if(PORTBbits.RB0 == 0){//Incrementar minutos
            while(PORTBbits.RB0 == 0);//Incrementar minutos
            minutos++;
        }
        else if(PORTBbits.RB1 == 0){//Incrementar horas
            while(PORTBbits.RB1 == 0);//Incrementar horas
            horas++;
        }
        else if(PORTBbits.RB2 == 0){//Incrementar dias
            while(PORTBbits.RB2 == 0);//Incrementar dias
            dias++;
        }
        else if(PORTBbits.RB3 == 0){//Incrementar meses
            while(PORTBbits.RB3 == 0);//Incrementar meses
            meses++;
        }
        else if(PORTBbits.RB4 == 0){//Incrementar biciestos
            while(PORTBbits.RB4 == 0);//Incrementar biciestos
            biciestos++;
        }
        INTCONbits.RBIF = 0;
    }
    return;
}
//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
    Lcd_Init8(); //Inicializamos la LCD
    Lcd_Clear8(); //Limpiamos la LCD
    ds3231_escribir = (ds3231_direccion<<1);
    ds3231_leer = (ds3231_direccion<<1) | (1<<0);
    while(1){
        I2C_Master_Start(); //Le escibimos el contador al Esclavo1
        I2C_Master_Write(0x50);
        I2C_Master_Write(contador);
        I2C_Master_Stop();
        __delay_ms(50);
       
        I2C_Master_Start(); //Leemos el ADC del Esclavo1
        I2C_Master_Write(0x51);
        ADC = I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(50);
        
        //Le escibimos al reloj los valores de minutos, horas, dias, meses y años
        escribir_reloj(ds3231_direccion_minutos, int_a_bcd(minutos));
        escribir_reloj(ds3231_direccion_horas, int_a_bcd(horas));
        escribir_reloj(ds3231_direccion_dias, int_a_bcd(dias));
        escribir_reloj(ds3231_direccion_meses, int_a_bcd(meses));
        escribir_reloj(ds3231_direccion_biciestos, int_a_bcd(biciestos));
        
        ////Leemos del reloj los valores de minutos, horas, dias, meses y años
        segundos_decimal = bcd_a_decimal(leer_reloj(ds3231_direccion_segundos)); //Pasamos el dato BCD(seg) a formato decimal(segundos_decimal)
        __delay_ms(50);
        minutos_decimal = bcd_a_decimal(leer_reloj(ds3231_direccion_minutos)); //Pasamos el dato BCD(seg) a formato decimal(segundos_decimal)
        __delay_ms(50);
        horas_decimal = bcd_a_decimal(leer_reloj(ds3231_direccion_horas)); //Pasamos el dato BCD(seg) a formato decimal(segundos_decimal)
        __delay_ms(50);
        dia_decimal = bcd_a_decimal(leer_reloj(ds3231_direccion_dias)); //Pasamos el dato BCD(seg) a formato decimal(segundos_decimal)
        __delay_ms(50);
        meses_decimal = bcd_a_decimal(leer_reloj(ds3231_direccion_meses)); //Pasamos el dato BCD(seg) a formato decimal(segundos_decimal)
        __delay_ms(50);
        biciesto_decimal = bcd_a_decimal(leer_reloj(ds3231_direccion_biciestos)); //Pasamos el dato BCD(seg) a formato decimal(segundos_decimal)
        __delay_ms(50);
        
        //Pasamos cada valor leido a un string para presentarlo en la LCD
        sprintf(CadADC, "%d", ADC);
        sprintf(Segundo, "%d", segundos_decimal);
        sprintf(Minuto, "%d", minutos_decimal);
        sprintf(Hora, "%d", horas_decimal);
        sprintf(Dia, "%d", dia_decimal);
        sprintf(Mes, "%d", meses_decimal);
        sprintf(Biciesto, "%d", biciesto_decimal);
        
        Lcd_Clear8();
        Lcd_Set_Cursor8(1,1); //Definimos en donde queremos ver ADC en la LCD
        Lcd_Write_String8("ADC");

        Lcd_Set_Cursor8(2,1); //Definimos en donde queremos ver el ADC en la LCD
        Lcd_Write_String8(CadADC);
        
        if(biciesto_decimal <= 9){//Esto nos sirve para que haya 0 en los espacios vacíos mientras se llega a 10
            Lcd_Set_Cursor8(2,13); 
            Lcd_Write_String8("200");
            Lcd_Set_Cursor8(2,16); 
            Lcd_Write_String8(Biciesto);
        }
        else {
            Lcd_Set_Cursor8(2,13); 
            Lcd_Write_String8("20");
            Lcd_Set_Cursor8(2,15); 
            Lcd_Write_String8(Biciesto);
        }
        
        
        Lcd_Set_Cursor8(2,12); 
        Lcd_Write_String8("/");
        

        if(meses_decimal <= 9){ //Esto nos sirve para que haya 0 en los espacios vacíos mientras se llega a 10
            Lcd_Set_Cursor8(2,10); 
            Lcd_Write_String8("0");
            Lcd_Set_Cursor8(2,11);
            Lcd_Write_String8(Mes);
        }
        else {
            Lcd_Set_Cursor8(2,10); 
            Lcd_Write_String8(Mes);
        }
        
        
        Lcd_Set_Cursor8(2,9); 
        Lcd_Write_String8("/");
        
        
        if(dia_decimal <= 9){//Esto nos sirve para que haya 0 en los espacios vacíos mientras se llega a 10
            Lcd_Set_Cursor8(2,7); 
            Lcd_Write_String8("0");
            Lcd_Set_Cursor8(2,8); 
            Lcd_Write_String8(Dia);
        }
        else {
            Lcd_Set_Cursor8(2,7); 
            Lcd_Write_String8(Dia);
        }
        
        
        if(horas_decimal <= 9){//Esto nos sirve para que haya 0 en los espacios vacíos mientras se llega a 10
            Lcd_Set_Cursor8(1,9); 
            Lcd_Write_String8("0");
            Lcd_Set_Cursor8(1,10); 
            Lcd_Write_String8(Hora);
        }
        else {
            Lcd_Set_Cursor8(1,9); 
            Lcd_Write_String8(Hora);
        }
        
        
        Lcd_Set_Cursor8(1,11); //Definimos en donde queremos ver el adc2 en la LCD
        Lcd_Write_String8(":");
        
        
        if(minutos_decimal <= 9){//Esto nos sirve para que haya 0 en los espacios vacíos mientras se llega a 10
            Lcd_Set_Cursor8(1,12);
            Lcd_Write_String8("0");
            Lcd_Set_Cursor8(1,13);
            Lcd_Write_String8(Minuto);
        }
        else {
            Lcd_Set_Cursor8(1,12); 
            Lcd_Write_String8(Minuto);
        }
        
        
        Lcd_Set_Cursor8(1,14); 
        Lcd_Write_String8(":");
        
        
        if(segundos_decimal <= 9){//Esto nos sirve para que haya 0 en los espacios vacíos mientras se llega a 10
            Lcd_Set_Cursor8(1,15); 
            Lcd_Write_String8("0");
            Lcd_Set_Cursor8(1,16); 
            Lcd_Write_String8(Segundo);
        }
        else {
            Lcd_Set_Cursor8(1,15); 
            Lcd_Write_String8(Segundo);
        }
        
        __delay_ms(100);
        
        if(minutos_decimal > 59){//Le ponemos un límite al contador de minutos
            minutos = 0;
        }
        if(horas_decimal > 24){//Le ponemos un límite al contador de horas
            horas = 0;
        }
        if(dia_decimal > 31){//Le ponemos un límite al contador de dias
            dias = 0;
        }
        if(meses_decimal > 12){//Le ponemos un límite al contador de meses
            meses = 0;
        }
        /*if(biciesto_decimal > 59){
            biciestos = 0;
        }*/
        
        if (segundos_decimal >= 59){//Gestionamos las condiciones para que el reloj funcione como un reloj real
            minutos++;
        }
        if (minutos_decimal >= 59 && segundos_decimal >= 59){
            minutos = 0;
            horas++;
        }
        if (horas_decimal >= 24 && minutos_decimal >= 59 && segundos_decimal >= 59){
            minutos = 0;
            horas = 0;
            dias++;
        }
        if (dia_decimal >= 31 && horas_decimal >= 24 && minutos_decimal >= 59 && segundos_decimal >= 59){
            minutos = 0;
            horas = 0;
            dias = 0;
            meses++;
        }
        if (meses_decimal >= 12 && dia_decimal >= 31 && horas_decimal >= 24 && minutos_decimal >= 59 && segundos_decimal >= 59){
            minutos = 0;
            horas = 0;
            dias = 0;
            meses = 0;
            biciestos++;
        }
        
        contador++;   
        
    }
    return;
}
//*****************************************************************************
// Función de Inicialización
//*****************************************************************************
void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    
    //Configuramos los puertos como entradas/salidas
    TRISA = 0;
    TRISB = 0b11111111;
    TRISD = 0;
    TRISE = 0;
    
    //Limpiamos los puertos
    PORTA = 0;
    PORTB = 0;
    PORTD = 0;
    PORTE = 0;
    
    OPTION_REGbits.nRBPU = 0; //Habilitamos los PULLUPS
    IOCB = 0b11111111; //Habilitamos las interrupciones al cambiar de estaso el puerto B
    
    OSCCONbits.IRCF = 0b111; //8 MHz
    OSCCONbits.SCS = 1;
    
    INTCONbits.RBIF = 0; //Apagamos la bandera del puerto B
    INTCONbits.RBIE = 1; //Habilitamos la interrupción en el puerto B
    INTCONbits.GIE = 1;         // Habilitamos interrupciones
    INTCONbits.PEIE = 1;        // Habilitamos interrupciones PEIE
    
    I2C_Master_Init(100000);        // Inicializar Comunicación I2C
    return;
}

uint8_t bcd_a_decimal(uint8_t bcd) {//Función para convertir de bcd a decimal
    return ((bcd>>4)*10)+(bcd & 0x0F);
}

unsigned int int_a_bcd(unsigned int value) {//Función para convertir de decimal a bcd
    unsigned int bcd = 0;
    unsigned int multiplier = 1;

    while (value > 0) {
        unsigned int digit = value % 10;
        bcd += digit * multiplier;
        multiplier *= 16;  // Multiplicamos por 16 (2^4) para avanzar 4 bits en cada iteración
        value /= 10;
    }

    return bcd;
}

void escribir_reloj(unsigned char direccion, unsigned int valor){ //Función para escibirle valores al reloj
    I2C_Master_Start(); //Iniciamos la comunicación
    I2C_Master_Write(ds3231_escribir); //Le escribimos al I2C la dirección de escritura del sensor
    I2C_Master_Write(direccion); //Le escribimos al I2C la dirección del registro del sensor que queremos leer
    I2C_Master_Write(valor); //Le escibimos el valor deseado
    I2C_Master_Stop();//Paramos la comunicación
    __delay_ms(50);
}

unsigned int leer_reloj(unsigned char direccion){//Función para leer y obtener los valores al reloj
    unsigned int bcd = 0;
    I2C_Master_Start(); //Iniciamos la comunicación
    I2C_Master_Write(ds3231_escribir); //Le escribimos al I2C la dirección de escritura del sensor
    I2C_Master_Write(direccion); //Le escribimos al I2C la dirección del registro del sensor que queremos leer
    I2C_Master_RepeatedStart(); //Reseteamos la comunicación
    I2C_Master_Write(ds3231_leer); //Le escirbimos al I2C la dirección para leer el sensor
    bcd = I2C_Master_Read(0); //Obtenemos los segundos que nos da el sensor
    I2C_Master_Stop(); //Paramos la comunicación
    
    return bcd;
}
