/* 
 * Copyright (C) 2011 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Valentina Gaggero
 * email:  valentina.gaggero@iit.it
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
#ifndef _TIMERS_H_
#define _TIMERS_H_

// - doxy begin -------------------------------------------------------------------------------------------------------
/**	@file 	hal_timers.h
	@brief 	This file provides timer management functions.
	@author claudio.lorini@iit.it, marco.maggiali@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
	@date	-
*/

// - external dependencies --------------------------------------------------------------------------------------------
#include "dspic30_stdint.h"
#include "hal_errors.h"

// - public #define  --------------------------------------------------------------------------------------------------
#define TIMER_SOURCE_INTERNAL 0
#define TIMER_SOURCE_EXTERNAL 1


// - declaration of public user-defined types ------------------------------------------------------------------------- 

/**
  * @enum hal_timer_t: identify timer
*/
typedef enum
{
	hal_timerT1 = 1,
	hal_timerT2 = 2,
	hal_timerT3 = 3,
	hal_timerT4 = 4,
	hal_timerT5 = 5
}hal_timer_t;

/**
  * @enum hal_timer_intPriorityValue_t: rappresents the possible values for interrupt priority for timers
  * @brief 0 --> lowest priority, 7 --> highest
*/
typedef enum
{
	hal_timer_IntPrio0 = 0,
	hal_timer_IntPrio1 = 1,
	hal_timer_IntPrio2 = 2,
	hal_timer_IntPrio3 = 3,
	hal_timer_IntPrio4 = 4,
	hal_timer_IntPrio5 = 5,
	hal_timer_IntPrio6 = 6,
	hal_timer_IntPrio7 = 7
}hal_timer_intPriorityValue_t;  


/**
  * @enum hal_timer_prescalerValue_t: rappresents the possible values prescaler
*/
typedef enum
{
	hal_timer_prescaler1 = 0x0,		/*!< Prescaler 1:1 */
	hal_timer_prescaler8 = 0x1,		/*!< Prescaler 1:8 */
	hal_timer_prescaler64 = 0x2,	/*!< Prescaler 1:64 */
	hal_timer_prescaler256 = 0x3	/*!< Prescaler 1:256 */
}hal_timer_prescalerValue_t;  

/**
  * @typedef hal_timer_config_datastruct_t: timer data configuration
*/
typedef struct
{
	uint8_t 				gate_op:1; 	// if 0 ==> disable gate operation
	uint8_t 				idle:1;		// if 0 ==> stop the timer when cpu is in idle or sleep mode
	uint8_t 				source:1;	// TIMER_SOURCE_INTERNAL or TIMER_SOURCE_EXTERNAL
	hal_timer_prescalerValue_t  prescaler;	
	uint16_t 				match_value; // Contains the period match value to be stored into the PR register.
    void(*call_bkp_fn)(void); // callback invoked in timer interrupt.
}hal_timer_config_datastruct_t;

/**
	@brief Note: How to choose match value:
	fosc (frequency oscillator) = 10MHz
	PLL = 8;
	For do one insruction,it needs 4 clocks.
	for example, if we choose prescaler 64:

	freq = (fosc*PLL)/4 = 20MHz
	
	freq/prescaler = 5/16*10^6 Hz ==> DeltaT = 16/5*10^-6 sec ~= 3,2*10^-6 sec
	
	So, you need do choos a correct match value, in order to: 
	match_value * 3,2*10^-6 sec = desired clock period.	

	3906 -> 0,0125 sec 
	1953 -> 0,00625 sec
	312 -> 1ms
	156 -> 500us
	31 -> 100ns 
*/
  
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

/**
  * @fn		extern void hal_timer_config(hal_timer_t timer, hal_timer_config_datastruct_t *config_ptr);;
  * @brief  configure timer with values contained in *config_ptr. 
  * @param		timer: which timer to configure
  * @param		config_ptr: pointer to configuration structure
  * @retval none
  */
extern hal_result_t hal_timer_config(hal_timer_t timer, hal_timer_config_datastruct_t *config_ptr);


/**
  * @fn	extern void timer_start(timer_t timer);
  * @brief starts timer @timer
  * @param		timer indicates which timer must start
  * @retval none
  */
extern void hal_timer_start(hal_timer_t timer);

/**
  * @fn	extern void timer_stop(timer_t timer);
  * @brief stops timer @timer
  * @param		timer: indicates which timer must stop
  * @retval	none
  */
extern void hal_timer_stop(hal_timer_t timer);


/**
  * @fn	extern void hal_timer_matchValue_set(hal_timer_t timer, hal_timer_prescalerValue_t  prescaler, uint16_t match_value);
  * @brief	sets timer period by prescaler and match values. See the note at the beginning of the file.
  * @param		timer
  * @param		prescaler
  * @param		match_value
  * @retval none
  */
extern void hal_timer_matchValue_set(hal_timer_t timer, hal_timer_prescalerValue_t  prescaler, uint16_t match_value);

/**
  * @fn	extern void hal_timer_interrupt_config(hal_timer_t timer, hal_timer_intPriorityValue_t prio);
  * @brief	configure interrupt priority of timer @timer
  * @param		timer: timer to configure
  * @param		prio: value of timer priority (0--> lowest piority, 7--> highest)
  * @retval none
  */
extern void hal_timer_interrupt_config(hal_timer_t timer, hal_timer_intPriorityValue_t prio);

/**
  * @fn	extern void hal_timer_interrupt_ena(hal_timer_t timer);
  * @brief enables timer interrupt
  * @param timer
  * @retval none
  */
extern void hal_timer_interrupt_ena(hal_timer_t timer);

/**
  * @fn	extern void hal_timer_interrupt_disa(hal_timer_t timer);
  * @brief disables timer interrupt
  * @param	timer
  * @retval none
  */
extern void hal_timer_interrupt_disa(hal_timer_t timer);

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


