 /****************************************************
 *  BCB board - iCub Facility - 2013                 *
 *  ADC configuration                                *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/

#include "STM32F10x.h"
#include "adc.h"

uint16_t AD_last;                                   // Last converted value
uint8_t  AD_done = 0;                               // AD conversion done flag
int ADC_value[1];                                   // Last converted value

// ----------------------------------------------------------------------------
//  Function that initializes ADC
//  5 MHz operation, 20µs startup time, 600ns track and hold time
// ----------------------------------------------------------------------------
void ADC_Init (void) {

#ifndef __ADC_IRQ
    RCC->AHBENR |= ( 1UL <<  0);                    // enable periperal clock for DMA

    // DMA1 Channel1 configuration ---------------------------------------------
    DMA1_Channel1->CMAR  = (uint32_t)&ADC_value;    // set chn1 memory address
    DMA1_Channel1->CPAR  = (uint32_t)&(ADC1->DR);   // set chn1 peripheral address
    DMA1_Channel1->CNDTR = 1;                       // transmit 1 words
    DMA1_Channel1->CCR   = 0x000029A2;              // configure DMA channel
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);             // enable DMA1 Channel1 Interrupt
    DMA1_Channel1->CCR  |= (1 << 0);                // DMA Channel 1 enable
#endif

    // Setup and initialize ADC converter
    RCC->CFGR    |= 3 << 14;                        // ADC clk = PCLK2 / 8

    RCC->APB2ENR |= 1 << 9;                         // enable periperal clock for ADC1

    ADC1->SQR1  = 0;                                // Clear register, 1 conversion

    ADC1->SQR2  = 0;                                // Clear register

    ADC1->SQR3  = 0;                                // Clear register
    ADC1->SQR3  |= 10 <<  0;                        // 1. conversion = channel 10

    ADC1->SMPR1 |= 	5 <<  0;                        // sample time channel 10  55,5 cyc.

    ADC1->CR1   = ( 1 <<  8);                       // Scan mode on
    ADC1->CR2   = (1 <<  1)|                        // Continuous conversion mode
                  (7 << 17)|                        // select SWSTART
                  (1 << 20);                        // enable external Trigger

#ifndef __ADC_IRQ
  ADC1->CR2    |= ( 1 <<  8);           // DMA mode enable
#else
  ADC1->CR1    |= ( 1 <<  5);           // enable for EOC Interrupt
  NVIC_EnableIRQ(ADC1_2_IRQn);          // enable ADC Interrupt
#endif

  ADC1->CR2    |= ( 1 <<  0);           // ADC enable

  ADC1->CR2    |=  1 <<  3;             // Initialize calibration registers
  while (ADC1->CR2 & (1 << 3));         // Wait for init to finish
  ADC1->CR2    |=  1 <<  2;             // Start calibration
  while (ADC1->CR2 & (1 << 2));         // Wait for calibration to finish
}


// ----------------------------------------------------------------------------
//  start AD Conversion
// ----------------------------------------------------------------------------
void ADC_StartCnv (void) {

  ADC1->CR2    |=  (1UL << 22);           // Start A/D conversion
}


// ----------------------------------------------------------------------------
// stop AD Conversion
// ----------------------------------------------------------------------------
void ADC_StopCnv (void) {

  ADC1->CR2    &= ~(1UL << 22);           // Stop  A/D conversion
}


// ----------------------------------------------------------------------------
// get converted AD value
// ----------------------------------------------------------------------------
int *ADC_Conv(void){
	while (!(AD_done));                     // Wait for Conversion end
  AD_done = 0;

	return(ADC_value);
}



#ifndef __ADC_IRQ
// ----------------------------------------------------------------------------
// DMA IRQ: Executed when a transfer is completed
// ----------------------------------------------------------------------------
void DMA1_Channel1_IRQHandler(void) {

  if (DMA1->ISR & (1 << 1)) {            // TCIF1 interrupt?
    AD_done = 1;

    DMA1->IFCR  = (1 << 1);              // clear TCIF1 interrupt
  }
}

#endif


#ifdef __ADC_IRQ
// ----------------------------------------------------------------------------
// A/D IRQ: Executed when A/D Conversion is done
// ----------------------------------------------------------------------------
void ADC1_2_IRQHandler(void) {

  if (ADC1->SR & (1 << 1)) {            // ADC1 EOC interrupt?
    ADC_value = ADC1->DR;
    AD_done = 1;

    ADC1->SR &= ~(1 << 1);              // clear EOC interrupt
  }

}
#endif




#ifdef __ADC_IRQ
#error macro adc definita
#endif
