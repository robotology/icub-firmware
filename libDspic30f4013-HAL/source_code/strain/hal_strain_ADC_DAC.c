/* 
 * Copyright (C) 2007 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Claudio Lorini, Marco Randazzo, Valentina Gaggero
 * email:  claudio.lorini@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/
// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------
/*	@file 	hal_strain_ADC_DAC.c
	@brief 	This file implements functions to interact with ADC and DAC.
	@author claudio.lorini@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
	@date	-
*/



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include <spi.h>
#include <dsp.h>


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "dspic30_stdint.h"
#include "hal_errors.h"
#include "hal_utils.h"
#include "hal_strain_ADC_DAC.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
//#include "entity_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
uint16_t SPISTATValue; // Holds the information about SPI Enable/Disable


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static uint16_t SPICONValue; // Holds the information about SPI configuartion


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
void hal_strain_AD_setDAC(uint8_t PowerDownMode, uint16_t DACValue)
{
  uint16_t tmp; 

  //LATBbits.LATB6=1;  //led on

  if( DACValue > 0x3FF) 
  {
    hal_error_manage(ERR_DAC_VALUE2BIG);
    DACValue = 0x3FF;  // saturate to max
  }

//LATAbits.LATA11 = 1; // startconv must be longer than 3.2u  

  while(SPI1STATbits.SPITBF);  // transmission buffer full
  tmp = PDM_NORMAL & (DACValue<<2);
  hal_delay(30);
  LATDbits.LATD9  = 0; // SS DAC 
//LATAbits.LATA11 = 0;  
 
  WriteSPI1(tmp); 
  hal_delay(10); 
  LATDbits.LATD9 = 1;
  //*ADCValue=ReadSPI1(); // get ADC value

  //LATBbits.LATB6=0; //led off
}

void hal_strain_AD_getADC(uint16_t *ADCValue)
{
  //unsigned int tmp; 

LATBbits.LATB6=1;  //led on
LATAbits.LATA11 = 1; // startconv must be longer than 3.2u  

  while(SPI1STATbits.SPITBF);  // transmission buffer full
  //tmp = PDM_NORMAL & (DACValue<<2);
  hal_delay(50);
 //LATDbits.LATD9  = 0; // SS DAC 
LATBbits.LATB6=0; //led off
LATAbits.LATA11 = 0;  
 
  //WriteSPI1(tmp); 
  hal_delay(10); 
 //LATDbits.LATD9 = 1;
 *ADCValue=ReadSPI1(); // get ADC value

}

void hal_strain_AD_setDAC_getADC(uint8_t PowerDownMode, uint16_t DACValue, uint16_t *ADCValue)
// set the value for the DAC and get ADC value
// PAY ATTENCTION: do not change the sequence of operation: tight timing at work!
// Durata circa 3 usec
{
  uint16_t tmp; 
  if( DACValue > 0x3FF) 
  {
    hal_error_manage(ERR_DAC_VALUE2BIG);
    DACValue = 0x3FF;  // saturate to max
  }
LATBbits.LATB6=1;  //led on
LATAbits.LATA11 = 1; // startconv must be longer than 3.2u  

  while(SPI1STATbits.SPITBF);  // transmission buffer full
  tmp = PDM_NORMAL & (DACValue<<2);
  hal_delay(30);
 LATDbits.LATD9  = 0; // SS DAC 
LATAbits.LATA11 = 0;  
 LATBbits.LATB6=0; //led off
 
  WriteSPI1(tmp); 
  hal_delay(10); 
 LATDbits.LATD9 = 1;
 *ADCValue=ReadSPI1(); // get ADC value
}

void hal_strain_AD_init(void)
{
  // Configure SPI1 module to transmit 16 bit timer1 value in master mode
  SPICONValue  =  FRAME_ENABLE_OFF & FRAME_SYNC_OUTPUT &
    ENABLE_SDO_PIN & SPI_SMP_ON & SPI_CKE_OFF & SPI_MODE16_ON &
    SLAVE_ENABLE_OFF & CLK_POL_ACTIVE_HIGH &
    MASTER_ENABLE_ON & SEC_PRESCAL_1_1 & PRI_PRESCAL_1_1; 

  SPISTATValue  = SPI_ENABLE & SPI_IDLE_CON & SPI_RX_OVFLOW_CLR;
  ConfigIntSPI1(SPI_INT_PRI_4 &  SPI_INT_EN);

  OpenSPI1(SPICONValue,SPISTATValue );
}

//
// SPI IRQ Service Routines
// 
void __attribute__((interrupt, no_auto_psv)) _SPI1Interrupt(void)
{   
  IFS0bits.SPI1IF = 0;
}  

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



