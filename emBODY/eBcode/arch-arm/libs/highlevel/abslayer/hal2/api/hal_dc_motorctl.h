/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Maggiali
 * email:   marco.maggiali@iit.it
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

#ifndef _HAL_MOTORCTL_H_
#define _HAL_MOTORCTL_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_dc_motorctl.h
    @brief      This header file implements public interface to the hal pwm module.
    @author     marco.maggiali@iit.it
    @date       22/03/2013
**/

/** @defgroup doxy_group_hal_pwm HAL MOTORCL

    The HAL MOTORCL ...
	
	It is made for controlling DC motors by means of the advanced pwms TIM1 and TIM8.

    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

//#include "hal_base.h"
#include "hl_common.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/////////////////////// PWM Peripheral Input clock ////////////////////////////
#define CKTIM	((uint32_t )168000000uL) 	/* Silicon running at 60MHz Resolution: 1Hz */
/****	Power devices switching frequency  ****/
#define PWM_FREQ ((uint16_t) 25000) // in Hz  (N.b.: pattern type is center aligned)

/****    Deadtime Value   ****/
#define DEADTIME_NS	((uint16_t) 1000)  //in nsec; range is [0...3500] 

#define LOW_SIDE_POLARITY  TIM_OCIdleState_Reset

////////////////////// PWM Frequency ///////////////////////////////////

/****	 Pattern type is edge aligned  ****/

	#define PWM_PRSC ((uint8_t)0)

        /* Resolution: 1Hz */                            
	#define PWM_PERIOD ((uint16_t) (CKTIM / (uint32_t)(1 * PWM_FREQ *(PWM_PRSC+1)))) 

#define PWM_MINDUTY 20
/****	ADC IRQ-HANDLER frequency, related to PWM  ****/
#define REP_RATE (1)  // MUST BE ODD(N.b): Internal current loop is performed every 
                      //             (REP_RATE + 1)/(2*PWM_PERIOD) seconds.     
					     
////////////////////////////// MOTOR DEADTIME Value /////////////////////////////////
	#define MOTOR_DEADTIME  (uint16_t)((unsigned long long)CKTIM/2 \
          *(unsigned long long)DEADTIME_NS/1000000000uL)  
						
/** @typedef    typedef enum hal_motor_t 
    @brief      hal_pwm_status_t contains the states of the pwm peripheral.
 **/ 
typedef enum  
{ 
    motor1       = 0,        /**< motor 1 */
    motor2       = 1,        /**< motor 2 */  
	motor3       = 2,        /**< motor 3 */
    motor4       = 3,        /**< motor 4 */   

} hal_motor_t;


/** @typedef    typedef enum hal_pwm_status_t 
    @brief      hal_pwm_status_t contains the states of the pwm peripheral.
 **/ 
typedef enum  
{ 
    hal_motor_status_none       = 0,        /**< when the motor is yet to be initted */
    hal_motor_status_idle       = 1,        /**< when the motor is initted but not running yet */         
    hal_motor_status_enabled    = 2,        /**< when the motor is enabled */
    hal_motor_status_fault      = 3         /**< when the motor is in fault */
} hal_motor_status_t;




/** @typedef    typedef enum hal_pwm_prescaler_t;
    @brief      represents the possible prescaler values
 **/
typedef enum
{
    hal_pwm_prescalerAUTO = hal_NA08,     /**< Prescaler is internally selected to best match the countdown */
    hal_pwm_prescaler1    = 1,            /**< Prescaler 1:1 */
    hal_pwm_prescaler2    = 2,            /**< Prescaler 1:2 */
    hal_pwm_prescaler4    = 3,            /**< Prescaler 1:4 */
    hal_pwm_prescaler8    = 4,            /**< Prescaler 1:8 */
    hal_pwm_prescaler16   = 5,            /**< Prescaler 1:16 */
    hal_pwm_prescaler32   = 6,            /**< Prescaler 1:32 */
    hal_pwm_prescaler64   = 7,            /**< Prescaler 1:64 */
    hal_pwm_prescaler128  = 8,            /**< Prescaler 1:128 */
    hal_pwm_prescaler256  = 9             /**< Prescaler 1:256 */
} hal_pwm_prescaler_t; 


/** @typedef    typedef struct hal_pwm_cfg_t;
    @brief      contains configuration data of pwm peripheral.
 **/
typedef struct
{
    hal_pwm_prescaler_t           prescaler;          /**< the prescaler one wants to use */
    void* arg;                                          /**< argument of the callback                               */    
} hal_pwm_cfg_t;


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn			extern hal_result_t hal_motor_init(hal_motor_t id, const hal_pwm_cfg_t *cfg, hal_reltime_t *error)
    @brief  	This function initializes a motor. It can be called once per motor. 
				 
    @details    On ARM-STM32 architecture, the registers of the timers are 16 bits, and holds bot the counter and the prescaler.
                As a result, the precision and maximum range is chosen as follows: (prec, max) = (1000 us, 6400ms), 
                (100 us, 640ms), (10 us, 64ms), (1 us, 8ms). 
    @param      motor           The motor to initialise. 
    @param      cfg             The configuration. It cannot be NULL.

    @return 	hal_res_NOK_generic in case the motor isn't configured or not supported by board.
                hal_res_NOK_unsupported if the chosen pwm cfg is not supported
                hal_res_NOK_nullpointer if @e cfg is NULL
                hal_res_OK otherwise
  */
extern hal_result_t hal_motor_init(hal_motor_t id, const hal_pwm_cfg_t *cfg);

/** @fn			extern hal_result_t hal_motor_and_adc_init(hal_motor_t id, const hal_pwm_cfg_t *cfg, hal_reltime_t *error)
    @brief  	This function initializes all the motors and the adc. 
				 
    @details    On ARM-STM32 architecture, the registers of the timers are 16 bits, and holds bot the counter and the prescaler.
                As a result, the precision and maximum range is chosen as follows: (prec, max) = (1000 us, 6400ms), 
                (100 us, 640ms), (10 us, 64ms), (1 us, 8ms). 
    @param      motor           The motor to initialise. 
    @param      cfg             The configuration. It cannot be NULL.

    @return 	hal_res_NOK_generic in case the motor isn't configured or not supported by board.
                hal_res_NOK_unsupported if the chosen pwm cfg is not supported
                hal_res_NOK_nullpointer if @e cfg is NULL
                hal_res_OK otherwise
  */
extern hal_result_t hal_motor_and_adc_init(hal_motor_t id, const hal_pwm_cfg_t *cfg);


/** @fn         extern hal_result_t hal_motor_start(hal_motor_t id);
    @brief      starts motor @e id
    @param      motor           The motor to start. It must be initted before.
    @return     hal_res_NOK_generic in case the motor wasn't configured, else hal_res_OK
 **/
extern hal_result_t hal_motor_start(hal_motor_t id);


/** @fn         extern hal_result_t hal_motor_stop(hal_motor_t id);
    @brief      stops pwm @e id
    @param      id           The motor to stop. It must be initted and started before.
    @return     hal_res_NOK_generic in case the motor wasn't configured or started, else hal_res_OK
 **/
extern hal_result_t hal_motor_stop(hal_motor_t id);


/** @fn         extern hal_motor_status_t hal_motor_status_get(hal_motor_t id)
    @brief      gets the status of the motor
    @param      id                  the motor
    @return     motor's status
 **/
extern hal_motor_status_t hal_motor_status_get(hal_motor_t id);




/** @fn         extern hal_result_t hal_motor_pwmset(hal_motor_t id, int32_t pwmvalue)
    @brief      sets the pwm of the motor (positive is cw, negative is ccw). It is limited by the PWMPERIOD value 
    @param      id                  the motor
	@param      pwmvalue   the value of the dutycycle of the PWM. it could be positive or negative and is truncated to PWMPERIOD 
    @return     ok
 **/
extern hal_result_t hal_motor_pwmset(hal_motor_t id, int32_t pwmvalue);

 /** @fn         extern hal_result_t hal_pwm_interrupt_enable(hal_pwm_t id);
    @brief      enables pwm interrupt
    @param      id              the pwm
    @return     hal_res_NOK_generic if pwm is not initialized.
                hal_res_ok otherwise
 **/
extern hal_result_t hal_pwm_interrupt_enable(hal_motor_t id);


/** @fn         extern hal_result_t hal_pwm_interrupt_disable(hal_pwm_t id);
    @brief      disables pwm interrupt
    @param      id              teh pwm
    @return     hal_res_NOK_generic if pwm is not initialized.
                hal_res_ok otherwise
 
 **/
extern hal_result_t hal_pwm_interrupt_disable(hal_motor_t id);



extern int16_t hal_motor_pwmget(hal_motor_t id);
/** @}            
    end of group doxy_group_hal_pwm  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



