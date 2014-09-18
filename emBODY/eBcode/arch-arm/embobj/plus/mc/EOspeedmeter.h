
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

//#define ENC_INVALID 0x7FFFFFFF

#define SM_INVALID_FAULT       0x40
#define SM_TIMEOUT_FAULT       0x80
#define SM_HARDWARE_FAULT      0xC0

// - declaration of public user-defined types ------------------------------------------------------------------------- 

 

/** @typedef    typedef struct EOspeedmeter_hid EOspeedmeter
    @brief      EOpid is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  

typedef struct EOabsCalibratedEncoder_hid EOabsCalibratedEncoder;
typedef struct EOaxleVirtualEncoder_hid EOaxleVirtualEncoder;

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOabsCalibratedEncoder* eo_absCalibratedEncoder_New(void);
extern void eo_absCalibratedEncoder_SetSign(EOabsCalibratedEncoder*, int32_t sign);
extern int32_t eo_absCalibratedEncoder_Acquire(EOabsCalibratedEncoder*, int32_t pos, uint8_t error_mask);
extern uint8_t eo_absCalibratedEncoder_IsHardFault(EOabsCalibratedEncoder*);
extern uint8_t eo_absCalibratedEncoder_IsTimeoutFault(EOabsCalibratedEncoder*);
extern uint8_t eo_absCalibratedEncoder_IsInvalidFault(EOabsCalibratedEncoder*);
extern void eo_absCalibratedEncoder_ClearFaults(EOabsCalibratedEncoder* o);
extern eObool_t eo_absCalibratedEncoder_IsOk(EOabsCalibratedEncoder*);
extern void eo_absCalibratedEncoder_Calibrate(EOabsCalibratedEncoder*, int32_t offset);
#ifndef USE_2FOC_FAST_ENCODER
extern int32_t eo_absCalibratedEncoder_GetVel(EOabsCalibratedEncoder* o);
#endif

#ifdef USE_2FOC_FAST_ENCODER
extern EOaxleVirtualEncoder* eo_axleVirtualEncoder_New(void);
extern void eo_axleVirtualEncoder_SetSign(EOaxleVirtualEncoder*, eObool_t inverted);
extern void eo_axleVirtualEncoder_Acquire(EOaxleVirtualEncoder*, int32_t axle_abs_pos, int32_t axle_virt_pos, int32_t axle_virt_vel);
extern int32_t eo_axleVirtualEncoder_GetPos(EOaxleVirtualEncoder*);
extern int32_t eo_axleVirtualEncoder_GetVel(EOaxleVirtualEncoder*);
#endif

/** @}            
    end of group eo_pid  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

