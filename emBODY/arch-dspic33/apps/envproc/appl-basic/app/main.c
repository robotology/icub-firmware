/* 
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: 	Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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

/*
    @file       main.c
    @brief      this file contains the very basic main of a project for boards with dspic33fj128mc802.
    @author     valentina.gaggero@iit.it / marco.accame@iit.it
    @date       10/03/2011
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "string.h"
#include "stdlib.h"
#include "stdint.h"
#include "p33fxxxx.h"
#include <libpic30.h>

#include "eEbasicStorage.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section 



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define 			my_CONFIG_REGISTERS_BASIC(dummy)                          		\
                                                                                    \
                    /* no Boot sector and write protection disabled */              \
                    _FBS (BSS_NO_FLASH & BWRP_WRPROTECT_OFF);                       \
                    /* no secure sector and write protection disabled */            \
                    _FSS (RSS_NO_RAM & SSS_NO_FLASH & SWRP_WRPROTECT_OFF);          \
                    /* Code protection disabled */                                  \
                    _FGS(GSS_OFF);                                                  \
                    /* Clock switching disabled Fail safe Clock Monitor disabled  */ \
                    _FOSCSEL(IESO_OFF & FNOSC_FRC);                                 \
                    /* Clock switching and clock monitor disabled, */               \
                    /* EC external clock, OSCOUT as IOPIN */                        \
                    _FOSC(FCKSM_CSECMD & POSCMD_EC & OSCIOFNC_ON);                  \
                    /* Turn off Watchdog Timer */                                   \
                    _FWDT (FWDTEN_OFF);                                             \
                    /* PWM mode is Port registers PWM high & low active high */     \
                    /* alternate I2C mapped to SDA1/SCL1 */                         \
                    /* FPOR power on reset 128ms  */                                \
                    _FPOR (PWMPIN_ON & HPOL_ON & LPOL_ON & FPWRT_PWR128);           \
                    /* Use PGC3/PGD3 for programming and debugging */               \
                    _FICD (ICS_PGD3 & JTAGEN_OFF);                                  


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section
 

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
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of main function
// --------------------------------------------------------------------------------------------------------------------


// whatever is needed to initialise the basic registers

my_CONFIG_REGISTERS_BASIC(0);

int main(void)
{
 	volatile uint32_t index = 0;
	volatile eEbasicstrg_res_t res;
	uint32_t canadr;

	res = ee_basicstrg_init();
	res = ee_basicstrg_canaddress_get(&canadr); 
	res = res;

    while(1)
    {
       	index ++; 
		canadr = canadr;
    }
    return(0);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

