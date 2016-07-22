
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOmcController_H_
#define _EOmcController_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOmcController.h
    @brief      This header file implements public interface to a motor controller.
    @author     alessandro.scalzo@iit.it
    @date       27/03/2012
**/

/** @defgroup eo_mcController Object EOmcController
    Does something.
     
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoMotionControl.h"


// - public #define  --------------------------------------------------------------------------------------------------

//#define MC_CAN_DEBUG

// - declaration of public user-defined types ------------------------------------------------------------------------- 

// marco.accame: use eOmc_ctrlboard_t instead
//typedef enum
//{
//    eomc_ctrlboard_DONTCARE                = 16,
//    eomc_ctrlboard_NO_CONTROL              = 0,
//    eomc_ctrlboard_ANKLE                   = 1,    //2FOC
//    eomc_ctrlboard_UPPERLEG                = 2,    //2FOC
//    eomc_ctrlboard_WAIST                   = 3,    //2FOC
//    eomc_ctrlboard_SHOULDER                = 4,    //2FOC
//    eomc_ctrlboard_HEAD_neckpitch_neckroll = 5,    //MC4plus
//    eomc_ctrlboard_HEAD_neckyaw_eyes       = 6,    //MC4plus
//    eomc_ctrlboard_FACE_eyelids_jaw        = 7,    //MC4plus
//    eomc_ctrlboard_4jointsNotCoupled       = 8,    //MC4plus this is the new name of eomc_ctrlboard_FACE_lips
//    eomc_ctrlboard_HAND_thumb              = 9,    //MC4plus
//    eomc_ctrlboard_HAND_2                  = 10,   //MC4plus
//    eomc_ctrlboard_FOREARM                 = 11,   //MC4plus
//    
//    // CER
//    
//    eomc_ctrlboard_CER_LOWER_ARM         = 12,   //MC4plus
//    eomc_ctrlboard_CER_HAND              = 14,   //MC2plus
//	eomc_ctrlboard_CER_WAIST             = 15,   //2FOC
//	eomc_ctrlboard_CER_UPPER_ARM         = 17,   //2FOC
//    eomc_ctrlboard_CER_BASE              = 21,   //2FOC
//    eomc_ctrlboard_CER_NECK              = 22    //MC4plus
//} eOeomc_ctrlboard_t;


typedef enum
{
    emscontroller_actuation_2FOC                = 0,
    emscontroller_actuation_LOCAL               = 1
} eOemscontroller_actuation_t;

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

 

/** @}            
    end of group eo_mcController  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

