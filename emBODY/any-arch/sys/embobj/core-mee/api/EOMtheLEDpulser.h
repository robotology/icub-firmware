/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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
#ifndef _EOMTHELEDPULSER_H_
#define _EOMTHELEDPULSER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOMtheLEDpulser.h
    @brief      This header file implements public interface to the LED pulser for the MEE
    @author     marco.accame@iit.it
    @date       02/05/2013
**/

/** @defgroup eom_theledpulser Singleton EOMtheLEDpulser
    The EOMtheLEDpulser is a type derived from #EOtheGPIOManager and allows management of GPIOs
    in a multi-tasking execution environment MEE.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "hal_led.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 
 

/** @typedef    typedef struct EOMtheLEDpulser_hid EOMtheLEDpulser
    @brief      EOMtheLEDpulser is an opaque struct. It is used to implement data abstraction for the GPIO manager 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOMtheLEDpulser_hid EOMtheLEDpulser;

typedef enum
{
    eom_ledpulser_led_zero  = 0,
    eom_ledpulser_led_one   = 1,
    eom_ledpulser_led_two   = 2,
    eom_ledpulser_led_three = 3,
    eom_ledpulser_led_four  = 4,
    eom_ledpulser_led_five  = 5
} eOmledpulser_led_t;

enum { eom_ledpulser_leds_number = 6 };

/**	@typedef    typedef struct eOmledpulser_cfg_t 
 	@brief      Contains the configuration for the eOmledpulser_cfg_t. 
 **/ 
typedef struct
{
    uint8_t             numberofleds;
    hal_led_t           leds[eom_ledpulser_leds_number];
} eOmledpulser_cfg_t;

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOmledpulser_cfg_t eom_ledpulser_DefaultCfg; // = { .numberofleds = 1, .leds = {hal_led0} };


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
/** @fn         extern EOMtheLEDpulser * eom_ledpulser_Initialise(EOtheGPIO *thegpio, const eOmledpulser_cfg_t *ledpulsercfg)
    @brief      Initialises the singleton gpio manager for the MEE.
    @arg        thegpio         The EOtheGPIO singleton which we want to manage.
    @arg        ledpulsercfg      The configuration. If NULL, then it is used eom_ledpulser_DefaultCfg.
    @return     The EOMtheLEDpulser singleton. If any problem, the #EOtheErrorManager issues a fatal error.
 **/
extern EOMtheLEDpulser * eom_ledpulser_Initialise(const eOmledpulser_cfg_t *ledpulsercfg); 


/** @fn         extern EOMtheLEDpulser * eom_ledpulser_GetHandle(void)
    @brief      Gets the initialised singleton.
    @return     The EOMtheLEDpulser singleton or NULL
 **/
extern EOMtheLEDpulser * eom_ledpulser_GetHandle(void); 

extern void eom_ledpulser_On(EOMtheLEDpulser* p, eOmledpulser_led_t id);

extern void eom_ledpulser_Off(EOMtheLEDpulser* p, eOmledpulser_led_t id);

extern void eom_ledpulser_Toggle(EOMtheLEDpulser* p, eOmledpulser_led_t id);

extern void eom_ledpulser_Start(EOMtheLEDpulser* p, eOmledpulser_led_t id, eOreltime_t pulseperiod, uint8_t pulsesnumberof); 

extern void eom_ledpulser_Stop(EOMtheLEDpulser* p, eOmledpulser_led_t id); 
 

/** @}            
    end of group eom_theledpulser  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



