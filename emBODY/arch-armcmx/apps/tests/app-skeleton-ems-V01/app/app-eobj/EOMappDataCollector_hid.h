// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOMDATACOLLECTOR_HID_H_
#define _EOMDATACOLLECTOR_HID_H_

// - doxy -------------------------------------------------------------------------------------------------------------

/* @file       object_hid.h
    @brief      This header file implements hidden interface to an entity.
    @author     marco.accame@iit.it
    @date       12/11/2009
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "EOMtask.h"
#include "EOethBaseModule.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
#include "EOMappDataCollector.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef enum
{
    eOm_appDataCollector_st__idle            = 0,
    eOm_appDataCollector_st__active          = 1,
    eOm_appDataCollector_st__collectingData  = 2
} eOm_appDataCollector_states_hid_t;



/** @struct     EOMappDataCollector_hid
    @brief      Hidden definition of  EOMappDataCollector object
 **/   
struct EOMappDataCollector_hid
{
    EOMappDataCollector_cfg_t           cfg;
    EOMtask                             *mytask;                /**< pointer to my task */
    eOm_appDataCollector_states_hid_t   st;
    EOethBaseModule                     *eth_mod;               /**< pointer to eth module. thi obj manages eth services*/
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



