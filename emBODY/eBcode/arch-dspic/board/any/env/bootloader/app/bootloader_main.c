/* 
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
    @file       bootloader_main.c
    @brief      this file contains the main of the bootloader for boards with dspic33fj128mc802.
    @author     valentina.gaggero@iit.it / marco.accame@iit.it
    @date       10/03/2011
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "hal_arch_dspic.h"
#include "bootloader.h"
#include "bootloader_sm.h"


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


#define hal_arch_dspic_base_CONFIG_REGISTERS_BOOTLOADER(dummy)                                      \
            /* - no Boot sector and write protection disabled */                                    \
            _FBS (BSS_NO_FLASH & BWRP_WRPROTECT_OFF);                                               \
            /* - no secure sector and write protection disabled  */                                 \
            _FSS (RSS_NO_RAM & SSS_NO_FLASH & SWRP_WRPROTECT_OFF);                                  \
            /* - Code protection disabled  */                                                       \
            _FGS(GSS_OFF);                                                                          \
            /* - Clock switching dsabled Fail safe Clock Monitor disabled */                        \
            /* - External clock with PLL x8 (10MHz*8->Fcycle=80/4=20MIPS) */                        \
            /* rem: _FOSCSEL(IESO_OFF & FNOSC_PRIPLL);  NON PARTO CON PLL   */                      \
            _FOSCSEL(IESO_OFF & FNOSC_FRC);                                                         \
            /* - Clock switching and clock monitor disabled, EC external clock, OSCOUT as IOPIN */  \
            /* rem: _FOSC(FCKSM_CSDCMD & POSCMD_EC & OSCIOFNC_ON); NON PARTO CON PLL */             \
            _FOSC(FCKSM_CSECMD & POSCMD_EC & OSCIOFNC_ON &IOL1WAY_OFF)                              \
            /* - Turn off Watchdog Timer */                                                         \
            _FWDT (FWDTEN_OFF);                                                                     \
            /* rem: _FWDT(WDT_ON & WDTPSA_512 & WDTPSB_1); // WD enabled 1:512*16  */               \
            /* - PWM mode is Port registers PWM high & low active high */                           \
            /* - alternate I2C mapped to SDA1/SCL1  */                                              \
            /* - FPOR power on reset 128ms */                                                       \
            _FPOR (PWMPIN_ON & HPOL_ON & LPOL_ON & FPWRT_PWR128);                                   \
            /* - Use PGC3/PGD3 for programming and debugging */                                     \
            _FICD (ICS_PGD3 & JTAGEN_OFF); /* & COE_ON ); //BKBUG_OFF  */                           
                                    


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

#if defined(UPDTBL)
hal_arch_dspic_base_CONFIG_REGISTERS_BASIC(0)
#else
hal_arch_dspic_base_CONFIG_REGISTERS_BOOTLOADER(0)
#endif

int main(void)
{
    bl_init();
    
    for(blsm_init();;)
    {
        blsm_event_t evt = blsm_get_evt();
        blsm_process_evt(evt);      
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
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

