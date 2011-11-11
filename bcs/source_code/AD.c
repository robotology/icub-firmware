#include "AD.h"
#include<p30f4011.h>
#include <adc10.h>

void AD_Init()
{
  unsigned int PinConfig;
  unsigned int  Adcon3_reg, Adcon2_reg, Adcon1_reg;
	
  ADCON1bits.ADON = 0;         // turn off ADC

  ConfigIntADC10(ADC_INT_DISABLE);

  PinConfig  =
    ENABLE_AN0_ANA & // [ADPCFG] enable 0..4 as analog inputs 
    ENABLE_AN1_ANA &
    ENABLE_AN2_ANA ;

  Adcon3_reg = ADC_SAMPLE_TIME_10 & 
    ADC_CONV_CLK_SYSTEM & 
    ADC_CONV_CLK_13Tcy;

  Adcon2_reg = ADC_VREF_AVDD_AVSS & 
    ADC_SCAN_OFF & // use ch selected in adchs
    ADC_ALT_BUF_OFF &
    ADC_ALT_INPUT_OFF &
    ADC_SAMPLES_PER_INT_1; // use one buffer, enable done reading

  Adcon1_reg = ADC_MODULE_OFF & 
    ADC_IDLE_CONTINUE & 
    ADC_FORMAT_INTG &
    ADC_CLK_MANUAL &
    ADC_AUTO_SAMPLING_OFF &
    ADC_SAMP_OFF;

  ADPCFG = PinConfig;

  // configure the input scan selection bits
  ADCON3 = Adcon3_reg;
  ADCON2 = Adcon2_reg;
  ADCON1 = Adcon1_reg;

  IFS0bits.ADIF = 0;
  ADCON1bits.ADON = 1;
}     
