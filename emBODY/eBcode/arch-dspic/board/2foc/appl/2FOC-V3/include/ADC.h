//   
//  ADC 10 bit syncronous with PWM rouines:
//  Measure Current & VDC-Link
//
//  Initialization, control and fault management 
//
#ifndef __ADC_H__
#define __ADC_H__

#include "MeasCurr.h"

extern SFRAC16 ADCBuffer[4];

extern volatile tMeasCurrParm MeasCurrParm;

int ADCGetVDCLink();
// read dclink voltage in [100mV] units
unsigned int ADCVDCLinkTo100mV(int vdc);
// pewrform offset calibration
void ADCDoOffsetCalibration();
void ADCDoGainCalibration();
// setup ADC for PWM sync and DMA
void ADCConfigPWMandDMAMode();
void ADCInterruptAndDMAEnable();
void ADCInterruptAndDMADisable();

#endif
