/* 
 * File:   ADC.h
 * Author: Black
 *
 * Created on 24 de julio de 2023, 03:09 PM
 */

#include <pic16f887.h>
#include <stdio.h>
#include <xc.h>
#include <stdint.h>
#include <stdlib.h>


#ifndef ADC_H
#define	ADC_H

void adc_init(int channel);
int adc_read();
void adc_change_channel(int channel);
int adc_get_channel();
int map (int val, int minx, int maxx, int miny, int maxy);

#endif	/* ADC_H */



