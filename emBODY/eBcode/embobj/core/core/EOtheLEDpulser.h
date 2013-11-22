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
#ifndef _EOTHELEDPULSER_H_
#define _EOTHELEDPULSER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOtheLEDpulser.h
    @brief      This header file implements public interface to the LED pulser
    @author     marco.accame@iit.it
    @date       02/05/2013
**/

/** @defgroup eom_theledpulser Singleton EOtheLEDpulser
    The EOtheLEDpulser allows to manage LEDs with features such as ON, OFF, toggle or pulse. It works on any EE as
    long as the relevant timer manager and callback manager are running.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 
 

/** @typedef    typedef struct EOtheLEDpulser_hid EOtheLEDpulser
    @brief      EOtheLEDpulser is an opaque struct. It is used to implement data abstraction for the GPIO manager 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOtheLEDpulser_hid EOtheLEDpulser;

typedef enum
{
    eo_ledpulser_led_zero  = 0,
    eo_ledpulser_led_one   = 1,
    eo_ledpulser_led_two   = 2,
    eo_ledpulser_led_three = 3,
    eo_ledpulser_led_four  = 4,
    eo_ledpulser_led_five  = 5,
    eo_ledpulser_led_none  = 255
} eOledpulser_led_t;

enum { eo_ledpulser_leds_number = 6 };


/**	@typedef    typedef struct eOledpulser_cfg_t 
 	@brief      Contains the configuration for the eOledpulser_cfg_t. 
 **/ 
typedef struct
{
    uint8_t                 led_enable_mask; /**< to enable leds one and five: 1 << eo_ledpulser_led_one | 1 << eo_ledpulser_led_five */
    eOint8_fp_uint8_cvoidp_t led_init;
    eOint8_fp_uint8_t       led_on;
    eOint8_fp_uint8_t       led_off;
    eOint8_fp_uint8_t       led_toggle;
} eOledpulser_cfg_t;

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOledpulser_cfg_t eo_ledpulser_DefaultCfg; // = { led_enable_mask = 1 << eo_ledpulser_led_zero };


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
/** @fn         extern EOtheLEDpulser * eo_ledpulser_Initialise(const eOledpulser_cfg_t *ledpulsercfg)
    @brief      Initialises the singleton LED pulser.
    @arg        ledpulsercfg      The configuration. If NULL, then it is used eo_ledpulser_DefaultCfg.
    @return     The EOtheLEDpulser singleton. If any problem, the EOtheErrorManager issues a fatal error.
 **/
extern EOtheLEDpulser * eo_ledpulser_Initialise(const eOledpulser_cfg_t *ledpulsercfg); 


/** @fn         extern EOtheLEDpulser * eo_ledpulser_GetHandle(void)
    @brief      Gets the initialised singleton.
    @return     The EOtheLEDpulser singleton or NULL
 **/
extern EOtheLEDpulser * eo_ledpulser_GetHandle(void); 

extern void eo_ledpulser_On(EOtheLEDpulser* p, eOledpulser_led_t id);

extern void eo_ledpulser_Off(EOtheLEDpulser* p, eOledpulser_led_t id);

extern void eo_ledpulser_Toggle(EOtheLEDpulser* p, eOledpulser_led_t id);

/** @fn         extern void eo_ledpulser_Start(EOtheLEDpulser* p, eOledpulser_led_t id, eOreltime_t pulseperiod, uint16_t pulsesnumberof)
    @brief      starts blinking teh specified led with a given period and a given number (or forever if pulsesnumberof is 0).
    @arg        p               the object
    @arg        id              teh id of teh led
    @arg        pulseperiod     teh period. to blink at 1 Hz use EOK_reltime1sec (1000*1000)
    @arg        pulsesnumberof  teh number of pulses. use 0 if the blink is forever.
    @return     The EOtheLEDpulser singleton or NULL
 **/
extern void eo_ledpulser_Start(EOtheLEDpulser* p, eOledpulser_led_t id, eOreltime_t pulseperiod, uint16_t pulsesnumberof); 

extern void eo_ledpulser_Stop(EOtheLEDpulser* p, eOledpulser_led_t id); 
 

/** @}            
    end of group eom_theledpulser  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



