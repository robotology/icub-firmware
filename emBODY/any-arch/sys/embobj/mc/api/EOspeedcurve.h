
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOSPEEDCURVE_H_
#define _EOSPEEDCURVE_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOtrajectory.h
    @brief      This header file implements public interface to a motor minimum jerk trajectory generator.
    @author     alessandro.scalzo@iit.it
    @date       27/03/2012
**/

/** @defgroup eo_trajectory Object EOtrajectory
    Does something.
     
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"


// - public #define  --------------------------------------------------------------------------------------------------



// - declaration of public user-defined types ------------------------------------------------------------------------- 

 

/** @typedef    typedef struct EOtrajectory_hid EOtrajectory
    @brief      EOtrajectory is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOspeedcurve_hid EOspeedcurve;


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
/** @fn         extern EOtrajectory* eo_trajectory_New(void)
    @brief      Creates a new trajectory object 
    @return     The pointer to the required object.
 **/
extern EOspeedcurve* eo_speedcurve_New(void);

extern void eo_speedcurve_SetReference(EOspeedcurve *o, int32_t p0, int32_t v0, int32_t vf, int32_t acc);

extern void eo_speedcurve_Stop(EOspeedcurve *o, int32_t p);

extern eObool_t eo_speedcurve_IsDone(EOspeedcurve* o);

extern int32_t eo_speedcurve_GetVel(EOspeedcurve* o);

extern void eo_speedcurve_Step(EOspeedcurve* o, int32_t* p, int32_t* v, int32_t *a);
//extern int32_t eo_speedcurve_Step2(EOtrajectory* o, int32_t *p, int32_t *v, int32_t *a)

/** @}            
    end of group eo_trajectory  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

