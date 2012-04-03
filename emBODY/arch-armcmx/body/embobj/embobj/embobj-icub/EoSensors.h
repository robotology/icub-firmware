
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOSENSORS_H_
#define _EOSENSORS_H_




/** @file       EoSensors.h
	@brief      This header file gives 
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_cevcwervcrev5555 Configuation of the vrfverver
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoUtilities.h"
#include "EoMeasures.h"


// - public #define  --------------------------------------------------------------------------------------------------





// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct eOsnsr_forcetorque_t
    @brief      eOsnsr_forcetorque_t contains measurements of force on xyz and of torque on 3 components 
    @warning    This struct must be of fixed and size and multiple of 4.
 **/
typedef struct                  // size is: 4+2*3+2*3+0 = 16 
{
    uint32_t                    identifier;         /**< the identifier of the origin of measure: so far only strain */ 
    eOmeas_force_t              force[3];           /**< the force split in x, y, and z components */
    eOmeas_torque_t             torque[3];          /**< the torque split in three components*/
} eOsnsr_forcetorque_t;         EO_VERIFYsizeof(eOsnsr_forcetorque_t, 16);


/** @typedef    typedef struct eOsnsr_aea_t
    @brief      eOsnsr_aea_t contains measurements of the absolute encoder whcih so far is managed only by teh ems 
    @warning    This struct must be of fixed and size and multiple of 4.
 **/
typedef struct                  // size is: 4+0 = 4
{
    uint32_t                    value;            // used with ... see hal_encoder.h for bitfield formatting  
} eOsnsr_aea_t;                 EO_VERIFYsizeof(eOsnsr_aea_t, 4);



/** @typedef    typedef uint8_t  eOsnsr_sensorId_t
    @brief      eOsnsr_sensorId_t contains the values required to identify a sensor board in robot.
 **/
typedef uint8_t  eOsnsr_sensorId_t;
    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section



/** @}            
    end of group eo_cevcwervcrev5555  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

