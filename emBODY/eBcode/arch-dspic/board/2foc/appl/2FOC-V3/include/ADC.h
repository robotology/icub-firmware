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

// Default ADC offset (center value)
#define ADC_RAW_DEFAULT_OFFSET 0x4000
#define ADC_RESOLUTION 10
#define ADC_WORD_SIZE 16

//#define TOL_ADC_OFFSET 1<<(ADC_WORD_SIZE-ADC_RESOLUTION)
//#define TOL_DIFF_CURR_DUE_TO_ADC_GAIN 2*TOL_ADC_OFFSET
#define TOL_ADC_OFFSET 20 // mA
#define TOL_DIFF_CURR_DUE_TO_ADC_GAIN ((10*TOL_ADC_OFFSET)^2)

typedef enum {
    inputChannel_TA_AN0      = 0,
    inputChannel_TC_AN1      = 1,
    inputChannel_VDCLINK_AN2 = 2
} inputChannel_t;

typedef enum {
    triggerType_MANUAL = 0,
    triggerType_PWM = 3
} triggerType_t;

typedef enum {
    DMAchannel_DMA0 = 0,
    DMAchannel_DMA3 = 3
} DMAchannel_t;

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
void ADCInterruptAndDMAEnable(DMAchannel_t DMAchannel);
void ADCInterruptAndDMADisable(DMAchannel_t DMAchannel);

#endif
