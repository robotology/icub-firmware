
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

#define USE_2FOC_FAST_ENCODER

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
 
 
extern EOspeedmeter* eo_speedmeter_New(int32_t impulse_per_revolution);

extern void eo_speedometer_EncoderValid(EOspeedmeter* o, int32_t encoder);
extern void eo_speedometer_EncoderError(EOspeedmeter* o);

extern void eo_speedometer_ReadSpeed(EOspeedmeter* o, int32_t speed);

//extern int32_t eo_speedometer_GetPosition(EOspeedmeter* o);
extern int32_t eo_speedometer_GetDistance(EOspeedmeter* o);
extern int32_t eo_speedometer_GetVelocity(EOspeedmeter* o);

extern void eo_speedometer_EncoderCalibrate(EOspeedmeter* o, int32_t new_calibration);


/** @}            
    end of group eo_pid  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

