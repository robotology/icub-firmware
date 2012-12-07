
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOSPEEDMETER_H_
#define _EOSPEEDMETER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOspeedmeter.h
    @brief      This header file implements public interface to axis speed measure from slow encoder.
    @author     alessandro.scalzo@iit.it
    @date       19/05/2012
**/

/** @defgroup eo_speedmeter Object EOspeedmeter
    Does something.
     
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"


// - public #define  --------------------------------------------------------------------------------------------------

#define ENC_INVALID -1

//#define USE_2FOC_FAST_ENCODER

#define SPEED_2FOC_TO_EMS(speed) (5*((int32_t)(speed)))

// - declaration of public user-defined types ------------------------------------------------------------------------- 

 

/** @typedef    typedef struct EOspeedmeter_hid EOspeedmeter
    @brief      EOpid is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOspeedmeter_hid EOspeedmeter;


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
extern EOspeedmeter* eo_speedmeter_New(void);
extern void eo_speedometer_Reset(EOspeedmeter* o);
extern eObool_t eo_speedometer_IsHardFault(EOspeedmeter* o);
extern eObool_t eo_speedometer_IsStarted(EOspeedmeter* o);

extern void eo_speedometer_SlowEncoderRead(EOspeedmeter* o, int32_t position);

#ifdef USE_2FOC_FAST_ENCODER
extern void eo_speedometer_FastEncoderRead(EOspeedmeter* o, int32_t velocity);
#endif

extern int32_t eo_speedometer_GetDistance(EOspeedmeter* o);
extern int32_t eo_speedometer_GetVelocity(EOspeedmeter* o);

/** @}            
    end of group eo_pid  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

