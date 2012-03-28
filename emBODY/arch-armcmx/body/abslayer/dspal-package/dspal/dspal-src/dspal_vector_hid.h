
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _DSPAL_VECTOR_HID_H_
#define _DSPAL_VECTOR_HID_H_


/* @file       dspal_vector_hid.h
    @brief      This header file implements hidden interface to parts of hal specifics of architecture
    @author     marco.accame@iit.it
    @date       09/19/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "dspal_base.h"
#include "dspal_base_hid.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "dspal_vector.h"


// - #define used with hidden structs ---------------------------------------------------------------------------------


// - definition of hidden structs -------------------------------------------------------------------------------------
// empty-section

// - declaration of extern hidden variables ---------------------------------------------------------------------------



// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint32_t dspal_vector_hid_getsize(const dspal_cfg_t *cfg);
extern dspal_result_t dspal_vector_hid_setmem(const dspal_cfg_t *cfg, uint32_t *memory);


// - definition of extern hidden inline functions ---------------------------------------------------------------------
// empty-section

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




