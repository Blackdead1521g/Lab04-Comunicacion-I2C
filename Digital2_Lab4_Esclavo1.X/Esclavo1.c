/* Archivo: Esclavo1.c
 * Dispositivo: PIC16F887
 * Autor: Kevin Alarc�n
 * Compilador: XC8(v2.40), MPLABX V6.05
 * 
 * 
 * Programa: Realizar un contador con ADC y enviarlo al master por comunicaci�n I2C
 * Hardware: Potenciometro.
 * 
 * Creado: 08 de agosto, 2023
 * �ltima modificaci�n: 11 de agosto, 2023
 */
//*****************************************************************************
// Palabra de configuraci�n
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
// Definici�n e importaci�n de librer�as
//*****************************************************************************
#include <stdint.h>
#include <pic16f887.h>
#include "I2C.h"
#include "ADC.h"
#include <xc.h>
//*****************************************************************************
// Definici�n de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000
uint8_t z;
uint8_t dato;
uint8_t POT;
//*****************************************************************************
// Definici�n de funciones para que se puedan colocar despu�s del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);
//*****************************************************************************
// C�digo de Interrupci�n 
//*****************************************************************************
void __interrupt() isr(void){
    //Interrupci�n del ADC
    if (PIR1bits.ADIF) {
        POT = ADRESH; //Le asignamos a n�mero el valor del ADC
        
        PIR1bits.ADIF = 0;
    }
    
   if(PIR1bits.SSPIF == 1){ //Revisamos si ya hay datos que recibir

        SSPCONbits.CKP = 0;
       
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)){
            z = SSPBUF;                 // Read the previous value to clear the buffer
            SSPCONbits.SSPOV = 0;       // Clear the overflow flag
            SSPCONbits.WCOL = 0;        // Clear the collision bit
            SSPCONbits.CKP = 1;         // Enables SCL (Clock)
        }

        if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW) { //Condici�n para recibir datos del maestro 
            //__delay_us(7);
            z = SSPBUF;                 // Lectura del SSBUF para limpiar el buffer y la bandera BF
            //__delay_us(2);
            PIR1bits.SSPIF = 0;         // Limpia bandera de interrupci�n recepci�n/transmisi�n SSP
            SSPCONbits.CKP = 1;         // Habilita entrada de pulsos de reloj SCL
            while(!SSPSTATbits.BF);     // Esperar a que la recepci�n se complete
            PORTD = SSPBUF;             // Guardar en el PORTD el valor del buffer de recepci�n
            __delay_us(250);
            
        }else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){ //Condici�n para escibirle al maestro
            z = SSPBUF;
            BF = 0;
            SSPBUF = PORTB;
            SSPCONbits.CKP = 1;
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
       
        PIR1bits.SSPIF = 0;    
    }
}
//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
    //*************************************************************************
    // Loop infinito
    //*************************************************************************
    while(1){
        PORTB = POT; //Le asignamos al puertoB el valor del ADC
        
        if (ADCON0bits.GO == 0) { // Si la lectura del ADC se desactiva

            __delay_us(1000); //Este es el tiempo que se dar� cada vez que se desactiva la lectura
            ADCON0bits.GO = 1; //Activamos la lectura del ADC
        }
        
       __delay_ms(500);
    }
    return;
}
//*****************************************************************************
// Funci�n de Inicializaci�n
//*****************************************************************************
void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    
    adc_init(0);
    //definir digitales
    ANSELbits.ANS0 = 1; //Seleccionamos solo los dos pines que utilizaremos como anal�gicos
    
    //Configuramos los puertos como entradas/salidas
    TRISA = 0b00000001;
    TRISB = 0;
    TRISD = 0;
    
    //Limpiamos los puertos
    PORTA = 0;
    PORTB = 0;
    PORTD = 0;
    
    OSCCONbits.IRCF = 0b111; //8 MHz
    OSCCONbits.SCS = 1;
    
    I2C_Slave_Init(0x50); // Inicializar Comunicaci�n I2C   
}

