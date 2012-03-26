
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOS_APPLEMBOBJ_SPEC_H_
#define _EOS_APPLEMBOBJ_SPEC_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       eos_applembobj_spec.h
    @brief      This header file implements ....
    @author     marco.accame@iit.it
    @date       01/11/2012
**/

/** @defgroup eupdaterinfo cedcew fcevw
    The embENV allows ...... 
 
    @todo acemor-facenda: do documentation.
    

    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "eEcommon.h"
#include "eOcommon.h"
#include "EOStheSystem.h"


// - public #define  --------------------------------------------------------------------------------------------------

#define EVT_1000TICKS       (0x1<<10)

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern void eos_applembobj_spec_hal_init(void);

extern void eos_applembobj_spec_sys_on_tick(void);

extern void eos_applembobj_spec_errman_on_error(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info);

extern void eos_applembobj_spec_foop_on_event(uint32_t evtpos, eOevent_t evtmsk);


extern void eos_applembobj_spec_user_init(void);


 

/** @}            
    end of group eupdaterinfo 
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


