/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
#ifndef _DSPAL_CONTROL_H_
#define _DSPAL_CONTROL_H_


/** @file       dspal_control.h
	@brief      This header file implements public interface to control part of the dsp abtraction layer 
	@author     marco.accame@iit.it
	@date       12/16/2010
**/

/* @defgroup dspal_control DSP abstraction layer
    cercecvecve.
    
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "dspal_base.h"




// - public #define  --------------------------------------------------------------------------------------------------

 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

  
/**
* @brief Instance structure for the Q15 PID Control.
*/
typedef struct
{
    dspal_q15_t a0;           /**< The derived gain, a0 = kp + ki + kd . */
    dspal_q31_t a1;           /**< The derived gain a1 = -kp - 2kd | kd. */
    dspal_q15_t state[3];       /**< The state array of length 3. */
    dspal_q15_t kp;           /**< The proportional gain. */
    dspal_q15_t ki;           /**< The integral gain. */
    dspal_q15_t kd;           /**< The derivative gain. */
} dspal_control_pid_q15_t;


/**
* @brief Instance structure for the Q31 PID Control.
*/
typedef struct
{
    dspal_q31_t a0;            /**< The derived gain, a0 = kp + ki + kd . */
    dspal_q31_t a1;            /**< The derived gain, a1 = -kp - 2kd. */
    dspal_q31_t a2;            /**< The derived gain, a2 = kd . */
    dspal_q31_t state[3];      /**< The state array of length 3. */
    dspal_q31_t kp;            /**< The proportional gain. */
    dspal_q31_t ki;            /**< The integral gain. */
    dspal_q31_t kd;            /**< The derivative gain. */
} dspal_control_pid_q31_t;


/**
* @brief Instance structure for the floating-point PID Control.
*/
typedef struct
{
    dspal_f32_t a0;                 /**< The derived gain, a0 = kp + ki + kd . */
    dspal_f32_t a1;                 /**< The derived gain, a1 = -kp - 2kd. */
    dspal_f32_t a2;                 /**< The derived gain, a2 = kd . */
    dspal_f32_t state[3];           /**< The state array of length 3. */
    dspal_f32_t kp;                 /**< The proportional gain. */
    dspal_f32_t ki;                 /**< The integral gain. */
    dspal_f32_t kd;                 /**< The derivative gain. */
} dspal_control_pid_f32_t;




                    
    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section (so far)


// - declaration of extern public functions ---------------------------------------------------------------------------



/**
* @brief  Initialization function for the floating-point PID Control.
* @param[in,out] *S      points to an instance of the PID structure.
* @param[in]     resetStateFlag  flag to reset the state. 0 = no change in state 1 = reset the state.
* @return none.
*/
extern void dspal_control_pid_init_f32(dspal_control_pid_f32_t * S, int32_t resetStateFlag);


/**
* @brief  Reset function for the floating-point PID Control.
* @param[in,out] *S is an instance of the floating-point PID Control structure
* @return none
*/
extern void dspal_control_pid_reset_f32(dspal_control_pid_f32_t * S);


/**
* @brief  Initialization function for the Q31 PID Control.
* @param[in,out] *S points to an instance of the Q15 PID structure.
* @param[in]     resetStateFlag  flag to reset the state. 0 = no change in state 1 = reset the state.
* @return none.
*/
extern void dspal_control_pid_init_q31(dspal_control_pid_q31_t * S, int32_t resetStateFlag);


/**
* @brief  Reset function for the Q31 PID Control.
* @param[in,out] *S points to an instance of the Q31 PID Control structure
* @return none
*/
extern void dspal_control_pid_reset_q31(dspal_control_pid_q31_t * S);


/**
* @brief  Initialization function for the Q15 PID Control.
* @param[in,out] *S points to an instance of the Q15 PID structure.
* @param[in] resetStateFlag  flag to reset the state. 0 = no change in state 1 = reset the state.
* @return none.
*/
extern void dspal_control_pid_init_q15(dspal_control_pid_q15_t * S, int32_t resetStateFlag);


/**
* @brief  Reset function for the Q15 PID Control.
* @param[in,out] *S points to an instance of the q15 PID Control structure
* @return none
*/
extern void dspal_control_pid_reset_q15(dspal_control_pid_q15_t * S);


/**
* @brief  Process function for the floating-point PID Control.
* @param[in,out] *S is an instance of the floating-point PID Control structure
* @param[in] in input sample to process
* @return out processed output sample.
*/
extern dspal_f32_t dspal_control_pid_f32(dspal_control_pid_f32_t * S, dspal_f32_t in);


/**
* @brief  Process function for the Q31 PID Control.
* @param[in,out] *S points to an instance of the Q31 PID Control structure
* @param[in] in input sample to process
* @return out processed output sample.
*
* <b>Scaling and Overflow Behavior:</b> 
* \par 
* The function is implemented using an internal 64-bit accumulator. 
* The accumulator has a 2.62 format and maintains full precision of the intermediate multiplication results but provides only a single guard bit. 
* Thus, if the accumulator result overflows it wraps around rather than clip. 
* In order to avoid overflows completely the input signal must be scaled down by 2 bits as there are four additions. 
* After all multiply-accumulates are performed, the 2.62 accumulator is truncated to 1.32 format and then saturated to 1.31 format. 
*/
extern dspal_q31_t dspal_control_pid_q31(dspal_control_pid_q31_t * S, dspal_q31_t in);


/**
* @brief  Process function for the Q15 PID Control.
* @param[in,out] *S points to an instance of the Q15 PID Control structure
* @param[in] in input sample to process
* @return out processed output sample.
*
* <b>Scaling and Overflow Behavior:</b> 
* \par 
* The function is implemented using a 64-bit internal accumulator. 
* Both Gains and state variables are represented in 1.15 format and multiplications yield a 2.30 result. 
* The 2.30 intermediate results are accumulated in a 64-bit accumulator in 34.30 format. 
* There is no risk of internal overflow with this approach and the full precision of intermediate multiplications is preserved. 
* After all additions have been performed, the accumulator is truncated to 34.15 format by discarding low 15 bits. 
* Lastly, the accumulator is saturated to yield a result in 1.15 format.
*/
extern dspal_q15_t dspal_control_pid_q15(dspal_control_pid_q15_t * S, dspal_q15_t in);
 


/**
*
* @brief  Floating-point Clarke transform
* @param[in]       Ia       input three-phase coordinate <code>a</code>
* @param[in]       Ib       input three-phase coordinate <code>b</code>
* @param[out]      *pIalpha points to output two-phase orthogonal vector axis alpha
* @param[out]      *pIbeta  points to output two-phase orthogonal vector axis beta
* @return none.
*/
extern void dspal_control_clarke_f32(dspal_f32_t Ia, dspal_f32_t Ib, dspal_f32_t * pIalpha, dspal_f32_t * pIbeta);


/**
* @brief  Clarke transform for Q31 version
* @param[in]       Ia       input three-phase coordinate <code>a</code>
* @param[in]       Ib       input three-phase coordinate <code>b</code>
* @param[out]      *pIalpha points to output two-phase orthogonal vector axis alpha
* @param[out]      *pIbeta  points to output two-phase orthogonal vector axis beta
* @return none.
*
* <b>Scaling and Overflow Behavior:</b>
* \par
* The function is implemented using an internal 32-bit accumulator.
* The accumulator maintains 1.31 format by truncating lower 31 bits of the intermediate multiplication in 2.62 format.
* There is saturation on the addition, hence there is no risk of overflow.
*/
extern void dspal_control_clarke_q31(dspal_q31_t Ia, dspal_q31_t Ib, dspal_q31_t * pIalpha, dspal_q31_t * pIbeta);


/**
* @brief  Floating-point Inverse Clarke transform
* @param[in]       Ialpha  input two-phase orthogonal vector axis alpha
* @param[in]       Ibeta   input two-phase orthogonal vector axis beta
* @param[out]      *pIa    points to output three-phase coordinate <code>a</code>
* @param[out]      *pIb    points to output three-phase coordinate <code>b</code>
* @return none.
*/
extern void dspal_control_clarke_inv_f32(dspal_f32_t Ialpha, dspal_f32_t Ibeta, dspal_f32_t * pIa, dspal_f32_t * pIb);


/**
* @brief  Inverse Clarke transform for Q31 version 
* @param[in]       Ialpha  input two-phase orthogonal vector axis alpha
* @param[in]       Ibeta   input two-phase orthogonal vector axis beta
* @param[out]      *pIa    points to output three-phase coordinate <code>a</code>
* @param[out]      *pIb    points to output three-phase coordinate <code>b</code>
* @return none.
*
* <b>Scaling and Overflow Behavior:</b>
* \par
* The function is implemented using an internal 32-bit accumulator.
* The accumulator maintains 1.31 format by truncating lower 31 bits of the intermediate multiplication in 2.62 format.
* There is saturation on the subtraction, hence there is no risk of overflow.
*/
extern void dspal_control_clarke_inv_q31(dspal_q31_t Ialpha, dspal_q31_t Ibeta, dspal_q31_t * pIa, dspal_q31_t * pIb);


/**
* @brief Floating-point Park transform
* @param[in]       Ialpha input two-phase vector coordinate alpha
* @param[in]       Ibeta  input two-phase vector coordinate beta
* @param[out]      *pId   points to output	rotor reference frame d
* @param[out]      *pIq   points to output	rotor reference frame q
* @param[in]       sinVal sine value of rotation angle theta
* @param[in]       cosVal cosine value of rotation angle theta
* @return none.
*
* The function implements the forward Park transform.
*
*/
extern void dspal_control_park_f32(dspal_f32_t Ialpha, dspal_f32_t Ibeta, dspal_f32_t * pId, dspal_f32_t * pIq, dspal_f32_t sinVal, dspal_f32_t cosVal);


/**
* @brief  Park transform for Q31 version 
* @param[in]       Ialpha input two-phase vector coordinate alpha
* @param[in]       Ibeta  input two-phase vector coordinate beta
* @param[out]      *pId   points to output rotor reference frame d
* @param[out]      *pIq   points to output rotor reference frame q
* @param[in]       sinVal sine value of rotation angle theta
* @param[in]       cosVal cosine value of rotation angle theta
* @return none.
*
* <b>Scaling and Overflow Behavior:</b>
* \par
* The function is implemented using an internal 32-bit accumulator.
* The accumulator maintains 1.31 format by truncating lower 31 bits of the intermediate multiplication in 2.62 format.
* There is saturation on the addition and subtraction, hence there is no risk of overflow.
*/
extern void dspal_control_park_q31(dspal_q31_t Ialpha, dspal_q31_t Ibeta, dspal_q31_t * pId, dspal_q31_t * pIq, dspal_q31_t sinVal, dspal_q31_t cosVal);


/**
* @brief  Floating-point Inverse Park transform
* @param[in]       Id        input coordinate of rotor reference frame d
* @param[in]       Iq        input coordinate of rotor reference frame q
* @param[out]      *pIalpha  points to output two-phase orthogonal vector axis alpha
* @param[out]      *pIbeta   points to output two-phase orthogonal vector axis beta
* @param[in]       sinVal    sine value of rotation angle theta
* @param[in]       cosVal    cosine value of rotation angle theta
* @return none.
*/
extern void dspal_control_park_inv_f32(dspal_f32_t Id, dspal_f32_t Iq, dspal_f32_t * pIalpha, dspal_f32_t * pIbeta, dspal_f32_t sinVal, dspal_f32_t cosVal);


/**
* @brief  Inverse Park transform for	Q31 version 
* @param[in]       Id        input coordinate of rotor reference frame d
* @param[in]       Iq        input coordinate of rotor reference frame q
* @param[out]      *pIalpha  points to output two-phase orthogonal vector axis alpha
* @param[out]      *pIbeta   points to output two-phase orthogonal vector axis beta
* @param[in]       sinVal    sine value of rotation angle theta
* @param[in]       cosVal    cosine value of rotation angle theta
* @return none.
*
* <b>Scaling and Overflow Behavior:</b>
* \par
* The function is implemented using an internal 32-bit accumulator.
* The accumulator maintains 1.31 format by truncating lower 31 bits of the intermediate multiplication in 2.62 format.
* There is saturation on the addition, hence there is no risk of overflow.
*/
extern void dspal_control_park_inv_q31(dspal_q31_t Id, dspal_q31_t Iq, dspal_q31_t * pIalpha, dspal_q31_t * pIbeta, dspal_q31_t sinVal, dspal_q31_t cosVal);

       
             
/* @}            
    end of group dspal_control  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

