//   
//  ADC 10 bit syncronous with PWM rouines:
//  Measure Current & VDC-Link
//
//  Initialization, control and fault management 
//
#ifndef __ADC_H__
#define __ADC_H__

#include "MeasCurr.h"

// number of samples taken to calculate zero calibration
#define ADC_CAL_N_SAMPLES 200

// Timeout for polling loops during zero calibration
#define ADC_CAL_TIMEOUT   1000

typedef struct {
    SFRAC16 adc1ch0[8];
    SFRAC16 adc1ch1[8];
    SFRAC16 adc1ch2[8];
    SFRAC16 adc1ch3[8];
} ADCBuffer_t;
extern ADCBuffer_t ADCBuffer;

extern volatile tMeasCurrParm MeasCurrParm;

int ADCGetVDCLink();
// read dclink voltage in [100mV] units
unsigned int ADCVDCLinkTo100mV(int vdc);
// pewrform offset calibration
void ADCDoOffsetCalibration();
// setup ADC for PWM sync and DMA
void ADCConfigPWMandDMAMode();
void ADCInterruptAndDMAEnable();
void ADCInterruptAndDMADisable();

#endif
