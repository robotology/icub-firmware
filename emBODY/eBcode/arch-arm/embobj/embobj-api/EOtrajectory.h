
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOTRAJECTORY_H_
#define _EOTRAJECTORY_H_

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
typedef struct EOtrajectory_hid EOtrajectory;


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
/** @fn         extern EOtrajectory* eo_trajectory_New(void)
    @brief      Creates a new trajectory object 
    @return     The pointer to the required object.
 **/
extern EOtrajectory* eo_trajectory_New(int32_t ticks_per_rev);

extern void eo_trajectory_SetPosMin(EOtrajectory *o, int32_t pos_min);
extern void eo_trajectory_SetPosMax(EOtrajectory *o, int32_t pos_max);
extern void eo_trajectory_SetVelMax(EOtrajectory *o, int32_t vel_max);

/** @fn         extern void eo_trajectory_SetReference(EOtrajectory *o, float p0, float pf, float v0, float speed)
    @brief      Sets minimum jerk trajectory new start and end points, initial and average speed. When the time is 
                expired, the function always returns the trajectory end point. 
    @param      o      The pointer to the trajectory object.
    @param      p0     The start position of the trajectory.         
    @param      pf     The end position of the trajectory.
    @param      v0     The inital velocity.
    @param      avg_speed  The average speed.
 **/
extern void eo_trajectory_SetPosReference(EOtrajectory *o, int32_t p1, int32_t avg_vel);
extern void eo_trajectory_SetVelReference(EOtrajectory *o, int32_t v1, int32_t avg_acc);
extern void eo_trajectory_BoostStart(EOtrajectory *o, int32_t v1, int32_t avg_acc);
extern void eo_trajectory_BoostTimeout(EOtrajectory *o);

/** @fn         extern float eo_trajectory_Step(EOtrajectory *o)
    @brief      Executes a trajectory step.
    @param      o  The pointer to the trajectory object.
    @return     The actual trajectory point value.
 **/
extern int8_t eo_trajectory_PosStep(EOtrajectory *o, float *p, float *v);
extern void eo_trajectory_Init(EOtrajectory *o, int32_t p0, int32_t v0, int32_t a0);

/** @fn         extern void eo_trajectory_Abort(EOtrajectory *o)
    @brief      Aborts the trajectory.
    @param      o  The pointer to the trajectory object.
 **/
extern void eo_trajectory_Stop(EOtrajectory *o, int32_t stop_acc);
extern void eo_trajectory_BoostStop(EOtrajectory *o);

//extern void eo_trajectory_Stop(EOtrajectory *o, float pos);

/** @fn         extern float eo_trajectory_GetPos(EOtrajectory *o)
    @brief      Get the actual trajectory position.
    @param      o  The pointer to the trajectory object.
    @return     The actual trajectory position.
 **/
extern int32_t eo_trajectory_GetPos(EOtrajectory *o);

/** @fn         extern float eo_trajectory_GetVelo(EOtrajectory *o)
    @brief      Get the actual trajectory velocity.
    @param      o  The pointer to the trajectory object.
    @return     The actual trajectory velocity.
 **/
extern int32_t eo_trajectory_GetVel(EOtrajectory *o);

extern eObool_t eo_trajectory_IsDone(EOtrajectory* o);

/** @}            
    end of group eo_trajectory  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

