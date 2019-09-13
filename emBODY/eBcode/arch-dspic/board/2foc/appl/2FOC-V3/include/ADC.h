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
#define ADC_CAL_N_SAMPLES 1024

// Timeout for polling loops during zero calibration
#define ADC_CAL_TIMEOUT   1000

extern SFRAC16 ADCBuffer[4];

extern volatile tMeasCurrParm MeasCurrParm;

int ADCGetVDCLink(void);
// read dclink voltage in [100mV] units
unsigned int ADCVDCLinkTo100mV(int vdc);
// perform offset calibration
void ADCDoOffsetCalibration(void);
void ADCDoGainCalibration(void);
BOOL Test_HES_ADC_offsetsNgains(void);
// setup ADC for PWM sync and DMA
void ADCConfigPWMandDMAMode(void);
void ADCInterruptAndDMAEnable(void);
void ADCInterruptAndDMADisable(void);
void MeasureAverageCurrentOnSinglePhase(int terminal,int measDuration,int* offset);

#endif
