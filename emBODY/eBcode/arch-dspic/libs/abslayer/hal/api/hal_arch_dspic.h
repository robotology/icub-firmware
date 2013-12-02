/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
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

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _HAL_ARCH_DSPIC_H_
#define _HAL_ARCH_DSPIC_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_arch_dspic.h
    @brief      This header file keeps public interface to the hal parts which are specific of dspic architecture.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       09/16/2011
**/

/** @defgroup hal_arch_dspic HAL ARCH DSPIC

    cecece
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"

#include "stdint.h"
#include "p33fxxxx.h"
#include <libpic30.h>



// - public #define  --------------------------------------------------------------------------------------------------

#define HAL_HAS_BASE
#define HAL_HAS_CAN
#define HAL_HAS_CRC
//#define HAL_HAS_DISPLAY
#define HAL_HAS_EEPROM
//#define HAL_HAS_ENCODER
//#define HAL_HAS_ETH
#define HAL_HAS_FLASH
#define HAL_HAS_GPIO
#define HAL_HAS_LED
#define HAL_HAS_SYS
#define HAL_HAS_TIMER
//#define HAL_HAS_TRACE
//#define HAL_HAS_WATCHDOG

/** @define     #define hal_arch_dspic_CONFIG_REGISTERS_BASIC(dummy)
    @brief      Configures the registers of the micro in their basic form. It is to be placed just before
                the @e main() function.
    @details    This basic configuration can be used on most boards, but if the user needs special feature, he/she
                should define a new macro by changing this one. Details of the macro are the following. TBD.
 **/
#define hal_arch_dspic_base_CONFIG_REGISTERS_BASIC(dummy)                           \
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
                    _FICD (ICS_PGD3 & JTAGEN_OFF);                                  \
                                    
  

/** this macro let you declare avariable of type TYPE called VAR_NAME in the SECTION of flash.
    Note1: SECTION must be defined in liker script
    Note2: aligned(N) macro specifies alignmen to a N-byte address */
#define hal_arch_dspic_eeprom_emfl_DECLARE_VAR_IN_EE(varname, type, section)   const type  __attribute__((space(prog))) __attribute__((aligned(_FLASH_PAGE*2))) __attribute__((__section__(section))) varname

/** This macro let you get flash address of VAR_NAME */
#define hal_arch_dspic_eeprom_emfl_GET_ADDR(addr, varname)                  _init_prog_address(addr, varname);

/** @define     #define hal_arch_dspic_int_priorityNONE     hal_int_priorityNONE
    @brief      The NONE priority. If used for an ISR, then it disables the interrupt
 **/                
#define hal_arch_dspic_int_priorityNONE     hal_int_priorityNONE

/** @define     #define hal_arch_dspic_int_priorityMAX      hal_int_priority00
    @brief      The maximum priority. If used for an ISR, then the ISR cannot be disabled in runtime anymore.
 **/
#define hal_arch_dspic_int_priorityMAX      hal_int_priority00


/** @define     #define hal_arch_dspic_int_priorityMIN      hal_int_priority07
    @brief      The minimum priority.
 **/
#define hal_arch_dspic_int_priorityMIN     hal_int_priority07


// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef enum hal_arch_arm_irqn_t
    @brief      expresses the irq number for the dspic33 architecture.
 **/
typedef enum 
{
    hal_arch_dspic_TIM1_IRQn            = 11,       /**< TIM1 global interrupt                          */
    hal_arch_dspic_DMA0_IRQn            = 12,       /**< DMA0 data transfer complete interrupt          */
    hal_arch_dspic_TIM2_IRQn            = 15,       /**< TIM2 global interrupt                          */
    hal_arch_dspic_TIM3_IRQn            = 16,       /**< TIM3 global interrupt                          */
    hal_arch_dspic_DMA1_IRQn            = 22,       /**< DMA1 data transfer complete interrupt          */
    hal_arch_dspic_TIM4_IRQn            = 35,       /**< TIM4 global interrupt                          */
    hal_arch_dspic_TIM5_IRQn            = 36,       /**< TIM5 global interrupt                          */
    hal_arch_dspic_C1RX_IRQn            = 42,       /**< ECAN1 RX data ready interrupt                  */
    hal_arch_dspic_CAN1_IRQn            = 43        /**< ECAN1 global interrupt                         */   
} hal_arch_dspic_irqn_t;


 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

//extern const hal_arch_dspic_crc16_polynomial_t hal_arch_dspic_crc16_polynomial_ccitt; // = 0x1021;


// - declaration of extern public functions ---------------------------------------------------------------------------

extern void hal_arch_dspic_delay(hal_time_t t);


/** @}            
    end of group hal_arch_dspic  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



