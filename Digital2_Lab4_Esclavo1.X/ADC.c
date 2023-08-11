/* Archivo: ADC.c
 * Dispositivo: PIC16F887
 * Autor: Kevin Alarcón
 * Compilador: XC8(v2.40), MPLABX V6.05
 * 
 * 
 * Programa: Librería de configuracion del ADC
 * Hardware: NA
 * 
 * Creado: 17 de julio, 2023
 * Última modificación: 21 de julio, 2023
 */

#include "ADC.h"

//Función para elegir en que canal iniciar el ADC
void adc_init(int channel){
    if(channel <= 13){ 
        ADCON0bits.CHS = channel;
    }
    else {
        ADCON0bits.CHS = 0b0000;
    }
    
    //Configuración ADC
    ADCON1bits.VCFG0 = 0; //Voltaje referenciado de 0V
    ADCON1bits.VCFG1 = 0; //Voltaje referenciado de 5V
    ADCON0bits.ADCS = 0b10; // Fosc/32
     
    ADCON1bits.ADFM = 0; //Justificado a la izquierda
    ADCON0bits.ADON = 1;//Encendemos el módulo del ADC
     
    //Banderas e interrupciones
    PIR1bits.ADIF = 0; //Apagamos la bandera del ADC
    INTCONbits.PEIE = 1; //Habilitar interrupciones periféricas
    PIE1bits.ADIE = 1; //Habilitar interrupciones del ADC
    INTCONbits.GIE = 1; //Habilitar interrupciones globales
    ADCON0bits.GO = 1; //Activamos la lectura del ADC
    return;
}

int adc_read(){
    return ADRESH;
}

void adc_change_channel(int channel){
    if(channel <= 13){ 
        ADCON0bits.CHS = channel;
    }
    else {
        ADCON0bits.CHS = 0b0000;
    }
    return;
    
}

int adc_get_channel(){
    char canal = ADCON0bits.CHS; //Le asignamos a la variable canal el número de canal del ADC
    return canal;
}

int map (int val, int minx, int maxx, int miny, int maxy){ //Función para mapear el valor del potenciometro a cualquier otro valor.
    int vo;
    vo = (long)(val-minx)*(maxy-miny)/(maxx-minx)+ miny; 
    return vo;
}

