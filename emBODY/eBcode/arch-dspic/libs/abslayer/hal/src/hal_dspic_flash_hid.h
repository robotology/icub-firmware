
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _HAL_DSPIC_FLASH_HID_H_
#define _HAL_DSPIC_FLASH_HID_H_


/* @file       hal_dspic_flash_hid.h
    @brief      This header file implements hidden interface to flash.
    @author     marco.accame@iit.it
    @date       09/12/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_flash.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------


// - declaration of extern hidden variables ---------------------------------------------------------------------------
// empty-section

// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint32_t hal_flash_hid_getsize(const hal_cfg_t *cfg);

extern hal_result_t hal_flash_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory);


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




