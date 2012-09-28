
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOPID_H_
#define _EOPID_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOpid.h
    @brief      This header file implements public interface to PID controller.
    @author     alessandro.scalzo@iit.it
    @date       27/03/2012
**/

/** @defgroup eo_pid Object EOpid
    Does something.
     
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"


// - public #define  --------------------------------------------------------------------------------------------------



// - declaration of public user-defined types ------------------------------------------------------------------------- 

 

/** @typedef    typedef struct EOpid_hid EOpid
    @brief      EOpid is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOpid_hid EOpid;


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
/** @fn         extern EOpid* eo_pid_New(void)
    @brief      Creates a new pid object 
    @return     The pointer to the required object.
 **/
extern EOpid* eo_pid_New(void);


/** @fn         extern void eo_pid_Init(EOpid *o, float Kp, float Kd, float Ki, float Ymax)
    @brief      Sets PID gain parameters and output limit.   
    @param      o     The pointer to the pid object.
    @param      Kp    The proportional gain.         
    @param      Kd    The derivative gain.
    @param      Ki    The integrative gain.
    @param      Ko    The constant offset.
    @param      Ymax  The output maximum value.
 **/
extern void eo_pid_Init(EOpid *o, float K, float Ki, float Kd, float Ymax, float Imax, float Yoff);

extern void eo_pid_SetPid(EOpid *o, float K, float Kd, float Ki, float Ymax, float Imax, float Yoff);

extern void eo_pid_GetStatus(EOpid *o, int32_t *pwm, int32_t *err);

extern uint8_t eo_pid_IsInitialized(EOpid *o);
extern void eo_pid_Reset(EOpid *o);

/** @fn         extern float eo_pid_PWM(EOpid *o, float En)
    @brief      Executes a PID control step.
    @param      o  The pointer to the pid object.
    @param      En The error between measure and setpoint.
    @return     The actual PWM output value.
 **/
//extern int16_t eo_pid_PWM(EOpid *o, int32_t En);
extern int16_t eo_pid_PWM2(EOpid *o, float En, float Vref, float Venc);

/** @}            
    end of group eo_pid  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

