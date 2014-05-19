
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
#include "EOemsControllerCfg.h"

// - public #define  --------------------------------------------------------------------------------------------------

#define ENC_INVALID 0x7FFFFFFF

#define SPEED_2FOC_TO_EMS(speed) (-10*((int32_t)(speed))) // 2FOC speed is tick x ms, but with 1/100 reduction => 1000/100 

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

#define OLD_STYLE_ENCODER

#ifdef OLD_STYLE_ENCODER
extern EOspeedmeter* eo_speedmeter_New(void);
extern void eo_speedometer_Calibrate(EOspeedmeter* o, int32_t offset);
extern eObool_t eo_speedometer_IsOk(EOspeedmeter* o);
//extern void eo_speedometer_Reset(EOspeedmeter* o);
extern eObool_t eo_speedometer_IsHardFault(EOspeedmeter* o);
extern void eo_speedometer_SetEncSign(EOspeedmeter* o, int32_t enc_sign);
extern void eo_speedometer_SlowEncoderRead(EOspeedmeter* o, int32_t position);
#ifdef USE_2FOC_FAST_ENCODER
extern void eo_speedometer_FastEncoderRead(EOspeedmeter* o, int32_t speed, int32_t fast_enc_pos);
#endif
extern int32_t eo_speedometer_GetDistance(EOspeedmeter* o);
extern int32_t eo_speedometer_GetVelocity(EOspeedmeter* o);
#else

extern EOslowenc* eo_slowenc_New(void);
extern void eo_slowenc_Calibrate(EOslowenc* o, int32_t offset);
extern eObool_t eo_slowenc_IsReady(EOslowenc* o);
extern eObool_t eo_slowenc_IsHardFault(EOslowenc* o);
extern void eo_slowenc_SetDirection(EOslowenc* o, int32_t sign);
extern void eo_slowenc_Acquire(EOslowenc* o, int32_t position);
extern int32_t eo_slowenc_GetPosition(EOspeedmeter* o);

extern EOfastenc* eo_fastenc_New(void);
extern eObool_t eo_fastenc_IsHardFault(EOfastenc* o);
extern void eo_fastenc_Acquire(EOfastenc* o, int32_t position, int32_t velocity);
extern int32_t eo_fastenc_GetPosition(EOfastenc* o);
extern int32_t eo_fastenc_GetVelocity(EOfastenc* o);

#endif

/** @}            
    end of group eo_pid  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

