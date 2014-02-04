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

#ifndef _HL_TIMER_H_
#define _HL_TIMER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_timer.h
    @brief      This header file implements timer utilities for hl library
    @author     marco.accame@iit.it
    @date       12/20/2013
**/

/** @defgroup doxy_group_hl_timer HL TIMER utilities

    The HL TIMER ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"
#include "hl_sys.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hl_timer_t 
    @brief      hl_timer_t contains the possible TIMER ports 
 **/
typedef enum
{
    hl_timer1  = 0,
    hl_timer2  = 1,
    hl_timer3  = 2,
    hl_timer4  = 3,
	hl_timer5  = 4,
    hl_timer6  = 5,
    hl_timer7  = 6,
    hl_timer8  = 7,
    hl_timer9  = 8,
    hl_timer10 = 9,
    hl_timer11 = 10,
    hl_timer12 = 11,
    hl_timer13 = 12,
    hl_timer14 = 13,
    hl_timer15 = 14,
    hl_timer16 = 15     
} hl_timer_t;

enum { hl_timers_number = 16 };

/** @typedef    typedef enum hl_timer_status_t 
    @brief      hl_timer_status_t contains the states of the timer
 **/ 
typedef enum  
{ 
    hl_timer_status_none        = 0,        /**< when the timer is yet to be initialized */
    hl_timer_status_idle        = 1,        /**< when the timer is initialized but not running yet */         
    hl_timer_status_running     = 2,        /**< when the timer is running in one-shot mode or forever mode */
    hl_timer_status_expired     = 3         /**< when the timer is running in one-shot mode and its countdown has expired */
} hl_timer_status_t;


/** @typedef    typedef enum hl_timer_mode_t 
    @brief      hl_timer_mode_t contains the modes of the timer
 **/ 
typedef enum  
{ 
    hl_timer_mode_oneshot  = 0,         
    hl_timer_mode_periodic = 1
} hl_timer_mode_t;


/** @typedef    typedef enum hl_timer_prescaler_t;
    @brief      represents the possible prescaler values
 **/
typedef enum
{
    hl_timer_prescalerAUTO  = hl_NA08,      /**< Prescaler is internally selected to best match the countdown */
    hl_timer_prescaler1     = 1,            /**< Prescaler 1:1 */
    hl_timer_prescaler2     = 2,            /**< Prescaler 1:2 */
    hl_timer_prescaler4     = 3,            /**< Prescaler 1:4 */
    hl_timer_prescaler8     = 4,            /**< Prescaler 1:8 */
    hl_timer_prescaler16    = 5,            /**< Prescaler 1:16 */
    hl_timer_prescaler32    = 6,            /**< Prescaler 1:32 */
    hl_timer_prescaler64    = 7,            /**< Prescaler 1:64 */
    hl_timer_prescaler128   = 8,            /**< Prescaler 1:128 */
    hl_timer_prescaler256   = 9             /**< Prescaler 1:256 */
} hl_timer_prescaler_t; 



/** @typedef    typedef struct hl_timer_mapping_t 
    @brief      hl_timer_mapping_t contains the mapping for all timers
 **/
typedef struct
{
    uint16_t            supported_mask;             /**< bit in position hl_timerx must be 1 if that timer is supported */
} hl_timer_mapping_t;


/** @typedef    typedef struct hl_timer_advcfg_opaque_t hl_timer_advcfg_t
    @brief      contains full configuration for timer as specified by the silicon vendor. 
                it is opaquely declared. see hl_arch.h for definition.                
 **/
typedef struct hl_timer_advcfg_opaque_t hl_timer_advcfg_t;


/** @typedef    typedef struct hl_timer_cfg_t;
    @brief      contains configuration data of timer peripheral.
 **/
typedef struct
{
    hl_reltime_t                    countdown;          /**< the countdown before the ISR executes the callback */
    hl_timer_mode_t                 mode;               /**< the mode of the timer: hl_timer_mode_oneshot or hl_timer_mode_periodic */
    hl_irqpriority_t                priority;           /**< if hl_irqpriorityNONE then there is no ISR and mode cannot be hl_timer_mode_oneshot */
    hl_callback_t                   callback;           /**< callback called by the ISR executed at the expiry of the timer    */
    void*                           arg;                /**< argument of the callback */ 
    const hl_timer_advcfg_t*        advcfg;             /**< used if not NULL. it overrides ... countdown. */    
} hl_timer_cfg_t;


 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hl_timer_cfg_t hl_timer_cfg_default; // = { .mode = hl_timer_mode_master, .behaviour = hl_timer_behaviour_polling, .speed = hl_timer_speed_400kbps, .ownaddress = 0, .advcfg = NULL };

// it must be externally declared.
extern const hl_timer_mapping_t* hl_timer_map;




// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn			extern hl_result_t hl_timer_init(hl_timer_t id, const hl_timer_cfg_t *cfg, hl_reltime_t *error)
    @brief  	This function initializes a timer. It can be called more times. If called on a running timer, it
                stops it first. If priority field of @e cfg has value different from hl_irqpriorityNONE,
                the function enable timer's ISR which is responsible of calling the associated callback function.
                The internal counter is set in order to satisfy the precision required by the specified countdown. 
                More accuracy means lower range of the countdown. 
    @details    On ARM-STM32 architecture, the registers of the timers are 16 bits, and hold both the counter and the prescaler.
                As a result, the precision and maximum range is chosen as follows: (prec, max) = (1000 us, 6400ms), 
                (100 us, 640ms), (10 us, 64ms), (1 us, 8ms). 
    @param      timer           The timer to initialise. 
    @param      cfg             The configuration. It cannot be NULL.
    @param      error           Gives back the error in microseconds between the target cfg->countdown and other settings in cfg.
                                This param can be NULL: in this case error is not calculated. In case cfg->advcfg, error will always 
                                be hl_NA32.
    @return 	hl_res_NOK_generic in case the timer isn't configured or not supported by board.
                hl_res_NOK_generic if timer is configured in hl_timer_mode_oneshot and its interrupt priority has value hl_irqpriorityNONE.
                hl_res_NOK_generic if countdown is zero.
                hl_res_NOK_nullpointer if @e cfg is NULL
                hl_res_OK otherwise
  */
extern hl_result_t hl_timer_init(hl_timer_t id, const hl_timer_cfg_t *cfg, hl_reltime_t *error);


/** @fn			extern hl_bool_t hl_timer_initted_is(hl_timer_t id)
    @brief  	this function tells if a timer has already been initted
    @param  	id              the id of timer
    @return 	hl_true or hl_false
  */
extern hl_bool_t hl_timer_initted_is(hl_timer_t id);


/** @fn         extern hl_result_t timer_start(hl_timer_t id);
    @brief      starts timer @e id
    @param      timer           The timer to start. It must be initted before.
    @return     hl_res_NOK_generic in case the timer wasn't configured, else hl_res_OK
 **/
extern hl_result_t hl_timer_start(hl_timer_t id);


/** @fn         extern hl_result_t timer_stop(hl_timer_t id);
    @brief      stops timer @e id
    @param      id           The timer to start. It must be initted and started before.
    @return     hl_res_NOK_generic in case the timer wasn't configured or started, else hl_res_OK
 **/
extern hl_result_t hl_timer_stop(hl_timer_t id);


/** @fn         extern hl_result_t hl_timer_remainingtime_get(hl_timer_t id, hl_reltime_t *remaining_time)
    @brief      reads timer's actual value
    @param      id                  the timer
    @param      remaining_time      Gives back the timer's value expressed microseconds. however, if the timer was initted with a custom
                                    cfg->advcfg, the value will always be hl_NA32 and return value hl_res_NOK_generic.
    @return     hl_res_NOK_generic if timer is not initialized or configured in hl_cfg,
                hl_res_OK otherwise.
 **/
extern hl_result_t hl_timer_remainingtime_get(hl_timer_t id, hl_reltime_t *remaining_time);


/** @fn         extern hl_timer_status_t hl_timer_status_get(hl_timer_t id)
    @brief      gets the status of the timer
    @param      id                  the timer
    @return     timer's status
 **/
extern hl_timer_status_t hl_timer_status_get(hl_timer_t id);


/** @fn         extern hl_result_t hl_timer_countdown_set(hl_timer_t id, hl_reltime_t countdown, hl_reltime_t *error);
    @brief      sets timer countdown. A running timer is stopped before setting the value and restarted afterwards. 
    @param      id              the timer
    @param      countdown       Timer's countdown expressed in microseconds.
    @param      error           The error in microseconds. This param can be NULL: in this case error is not calculated.
    @return     hl_res_NOK_generic / hl_res_OK
    @warning    This function is a wrapper of other functions. It does some controls and then calls hl_timer_init() with 
                proper parameters. This call is contained within hl_timer_stop() and hl_timer_start() if the timer was running
 **/
extern hl_result_t hl_timer_countdown_set(hl_timer_t id, hl_reltime_t countdown, hl_reltime_t *error);



/** @}            
    end of group doxy_group_hl_timer  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



