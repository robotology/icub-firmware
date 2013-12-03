
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOS_APPLEMBOBJ_INFO_H_
#define _EOS_APPLEMBOBJ_INFO_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       eos_applembobj_info.h
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
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern eEmoduleInfo_t               eos_applembobj_info_modinfo;

extern eOssystem_cfg_t              eos_applembobj_info_syscfg;

extern eOerrman_cfg_t               eos_applembobj_info_errman_cfg;

extern eOstimerman_cfg_t            eos_applembobj_info_thetimerman_cfg;

extern eOscallbackman_cfg_t         eos_applembobj_info_thecallbackman_cfg;

extern eOsfoop_cfg_t                eos_applembobj_info_thefoopcfg;





// - declaration of extern public functions ---------------------------------------------------------------------------

 

/** @}            
    end of group eupdaterinfo 
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


