
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOMEASURES_H_
#define _EOMEASURES_H_




/** @file       EoMeasures.h
	@brief      This header file gives 
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_c2e2rd3f3egfvr3 Configuation of  vfef ve e
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoUtilities.h"



// - public #define  --------------------------------------------------------------------------------------------------





// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef int32_t             eOmeas_position_t
    @brief      eOmeas_position_t contains the position values.
    @todo       how are they scaled? what is the measurement unit?
 **/
typedef int32_t             eOmeas_position_t;


/** @typedef    typedef int16_t             eOmeas_velocity_t
    @brief      eOmeas_velocity_t contains the velocity values.
    @todo       how are they scaled? what is the measurement unit?
 **/
typedef int16_t             eOmeas_velocity_t;


/** @typedef    typedef int16_t             eOmeas_acceleration_t
    @brief      eOmeas_acceleration_t contains the acceleration values.
    @todo       how are they scaled? what is the measurement unit?
 **/
typedef int16_t             eOmeas_acceleration_t;


/** @typedef    typedef int16_t             eOmeas_force_t
    @brief      eOmeas_force_t contains the torque values.
    @todo       how are they scaled? what is the measurement unit?
 **/
typedef int16_t             eOmeas_force_t;


/** @typedef    typedef int16_t             eOmeas_torque_t
    @brief      eOmeas_torque_t contains the torque values.
    @todo       how are they scaled? what is the measurement unit?
 **/
typedef int16_t             eOmeas_torque_t;


/** @typedef    typedef int16_t             eOmeas_current_t
    @brief      eOmeas_current_t contains the current values.
    @todo       how are they scaled? what is the measurement unit?
 **/
typedef int16_t             eOmeas_current_t;



    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section




/** @}            
    end of group eo_c2e2rd3f3egfvr3  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
