/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Maggiali, Marco Accame
 * email:   marco.maggiali@iit.it, marco.accame@iit.it
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

#ifndef _HAL_MOTOR_H_
#define _HAL_MOTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_motor.h
    @brief      This header file implements public interface to the hal dc motor control module.
    @author     marco.maggiali@iit.it, marco.accame@iit.it
    @date       25 jan 2016
**/

/** @defgroup doxy_group_hal_motor HAL Motor Control

    cevferver

    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"


// - public #define  --------------------------------------------------------------------------------------------------

/////////////////////// PWM Peripheral Input clock ////////////////////////////
#define CKTIM   ((uint32_t )168000000uL)    
/* Silicon running at 60MHz Resolution: 1Hz */

/****   Power devices switching frequency  ****/
#define PWM_FREQ ((uint16_t) 25000) 
// in Hz  (N.b.: pattern type is center aligned)

/****    Deadtime Value   ****/
#define DEADTIME_NS	((uint16_t) 1000)  
//in nsec; range is [0...3500] 

#define LOW_SIDE_POLARITY  TIM_OCIdleState_Reset

////////////////////// PWM Frequency ///////////////////////////////////

/****   Pattern type is edge aligned  ****/

#define PWM_PRSC ((uint8_t)0)

/* Resolution: 1Hz */                            
#define PWM_PERIOD ((uint16_t) (CKTIM / (uint32_t)(1 * PWM_FREQ *(PWM_PRSC+1)))) 

#define PWM_MINDUTY 20
/****   ADC IRQ-HANDLER frequency, related to PWM  ****/
#define REP_RATE (1)  
    // MUST BE ODD(N.b): Internal current loop is performed every 
    //             (REP_RATE + 1)/(2*PWM_PERIOD) seconds.     
     
////////////////////////////// MOTOR DEADTIME Value /////////////////////////////////
#define MOTOR_DEADTIME  (uint16_t)((unsigned long long)CKTIM/2 \
                        *(unsigned long long)DEADTIME_NS/1000000000uL)  
                        
                        
// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hal_motor_t 
    @brief      hal_motor_t contains the port name of the motor pwm peripheral.
 **/ 
typedef enum  
{ 
    hal_motor1          = 0,        /**< motor 1 */
    hal_motor2          = 1,        /**< motor 2 */  
    hal_motor3          = 2,        /**< motor 3 */
    hal_motor4          = 3,        /**< motor 4 */  
    hal_motorALL        = 254,      /**< all supported motors */
    hal_motorNONE       = 255       /**< no motor */       
} hal_motor_t;

enum { hal_motors_number = 4 };


///** @typedef    typedef enum hal_pwm_status_t 
//    @brief      hal_pwm_status_t contains the states of the pwm peripheral.
// **/ 
//typedef enum  
//{ 
//    hal_motor_status_none       = 0,        /**< when the motor is yet to be initted */
//    hal_motor_status_idle       = 1,        /**< when the motor is initted but not running yet */         
//    hal_motor_status_enabled    = 2,        /**< when the motor is enabled */
//    hal_motor_status_fault      = 3         /**< when the motor is in fault */
//} hal_motor_status_t;



/** @typedef    typedef struct hal_motor_cfg_t;
    @brief      contains configuration data of motor peripheral.
 **/
typedef struct
{ 
    uint32_t            dummy;
} hal_motor_cfg_t;


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern hal_boolval_t hal_motor_supported_is(hal_motor_t id)
    @brief      This function tells if the specified motor port is supported by the board. 
    @param      id              identifies port id 
    @return     hal_true or hal_false
  */
extern hal_boolval_t hal_motor_supported_is(hal_motor_t id);


/** @fn         extern hal_result_t hal_motor_extfault_handling_init(void)
    @brief      This function so far initialises all teh supported motors and also teh external faults.  
  */
extern hal_result_t hal_motor_init(hal_motor_t id, const hal_motor_cfg_t *cfg);


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


///** @fn         extern hal_motor_status_t hal_motor_status_get(hal_motor_t id)
//    @brief      gets the status of the motor
//    @param      id                  the motor
//    @return     motor's status
// **/
//extern hal_motor_status_t hal_motor_status_get(hal_motor_t id);


/** @fn         extern hal_result_t hal_motor_pwmset(hal_motor_t id, int32_t pwmvalue)
    @brief      sets the pwm of the motor (positive is cw, negative is ccw). It is limited by the PWMPERIOD value 
    @param      id                  the motor
    @param      pwmvalue   the value of the dutycycle of the PWM. it could be positive or negative and is truncated to PWMPERIOD 
    @return     ok
 **/
extern hal_result_t hal_motor_pwmset(hal_motor_t id, int16_t pwmvalue);


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

extern hal_bool_t hal_motor_external_fault_active(void);

extern int16_t hal_motor_pwmget(hal_motor_t id);

extern hal_bool_t hal_motor_externalfaulted(void);

extern hal_result_t hal_motor_enable(hal_motor_t id);

extern hal_result_t hal_motor_disable(hal_motor_t id);

extern hal_result_t hal_motor_reenable_break_interrupts(void);

/** @}            
    end of group doxy_group_hal_motor  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



