// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOMDATATRANSMITTER_HID_H_
#define _EOMDATATRANSMITTER_HID_H_

// - doxy -------------------------------------------------------------------------------------------------------------

/* @file       object_hid.h
    @brief      This header file implements hidden interface to an entity.
    @author     marco.accame@iit.it
    @date       12/11/2009
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "EOMtask.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
#include "EOMappDataTransmitter.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef enum
{
    eOm_appDataTransmitter_st__idle            = 0,
    eOm_appDataTransmitter_st__active          = 1,
    eOm_appDataTransmitter_st__transmitting     = 2
} eOm_appDataTransmitter_states_hid_t;


/** @struct     EOMdataTransmitter_hid
    @brief      Hidden definition of EOMdataTransmitter object
 **/  
 
struct EOMappDataTransmitter_hid
{
    EOMappDataTransmitter_cfg_t         cfg;        /**< object's configuration */
    EOMtask                             *mytask;    /**< pointer to my task */
    eOm_appDataTransmitter_states_hid_t st;         /**< object's state */
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



