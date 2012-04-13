
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOPID_H_
#define _EOPID_H_


/** @file       EOpid.h
    @brief      This header file implements public interface to a motor minimum jerk trajectory generator.
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

 

/** @typedef    typedef struct EOtrajectory_hid EOtrajectory
    @brief      EOtrajectory is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOpid_hid EOpid;


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
/** @fn         extern EOtrajectory* eo_trajectory_New(void)
    @brief      Creates a new trajectory object 
    @return     The pointer to the required object.
 **/
extern EOpid* eo_pid_New(void);


/** @fn         extern void eo_trajectory_Set(EOtrajectory *o, int32t position_start, int32_t position_end, uint32_t time_ms)
    @brief      Sets minimum jerk trajectory new start and end points, and the required time (milliseconds). When the time is 
                expired, the function always returns the trajectory end point. 
          
    @param      o               The pointer to the trajectory object.
    @param      position_start  The start position of the trajectory [turns/2^16].         
    @param      position_end    The end position of the trajectory [turns/2^16].
    @param      time_ms         The duration of the trajectory [ms].
 **/
extern void eo_pid_Set(EOpid *o, float Kp, float Kd, float Ki, float Ymax);


/** @fn         extern int32_t eo_trajectory_Step(EOtrajectory *o)
    @brief      Executes a trajectory step.
    @param      o               The pointer to the trajectory object.
    @return     The actual trajectory point value.
 **/
extern float eo_pid_Step(EOpid *o, float En);


/** @}            
    end of group eo_trajectory  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

