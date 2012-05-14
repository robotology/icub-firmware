
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
#include "icub/api/EoMotionControl.h"


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
extern void eo_pid_Configure(EOpid *o, eOmc_PID_t *c);
extern eObool_t eo_pid_IsConfigured(EOpid *o);
extern float eo_pid_GetOffset(EOpid *o);

extern void eo_pid_GetStatus(EOpid *o, float *pwm, float *err);
extern void eo_pid_Reset(EOpid *o);

extern float eo_pid_PWM(EOpid *o, float En);

/** @}            
    end of group eo_pid  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

