//   
//  ADC 10 bit syncronous with PWM rouines:
//  Measure Current & VDC-Link
//
//  Initialization, control and fault management 
//
#ifndef __ADC_H
#define __ADC_H

#include "UserTypes.h"
#include "MeasCurr.h"

// number of samples taken to calculate zero calibration
#define ADC_CAL_N_SAMPLES 200

// Timeout for polling loops during zero calibration
#define ADC_CAL_TIMEOUT   1000


// Globals are in 2foc.c
extern SFRAC16 ADCBuffer[4] __attribute__((space(dma))); 	
extern tMeasCurrParm MeasCurrParm;
extern int TargetDCbus;
extern int ADCPwmSampled;

SFRAC16 ADCGetVDCLink();
// read dclink voltage in [100mV] units
unsigned int ADCVDCLinkTo100mV(SFRAC16 tmp);
// pewrform offset calibration
void ADCDoOffsetCalibration();
// setup ADC for PWM sync and DMA
void ADCConfigPWMandDMAMode();
void ADCInterruptAndDMAEnable();
void ADCInterruptAndDMADisable();

#endif
