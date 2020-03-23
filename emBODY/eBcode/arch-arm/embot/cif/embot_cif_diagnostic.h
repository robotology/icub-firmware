
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - brief
//   it implements embobj C interface to the class embot::prot::eth::diagnostic::Node 
//

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_CIF_DIAGNOSTIC_H_
#define _EMBOT_CIF_DIAGNOSTIC_H_

#ifdef __cplusplus
extern "C" {
#endif
 

// - external dependencies --------------------------------------------------------------------------------------------

#include "EOtheErrorManager.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
   
   
extern void embot_cif_diagnostic_Init(); 

// of type eOerrman_fp_onerror_t and used by eo_errman_SetOnErrorHandler()
extern void embot_cif_diagnostic_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *errdes);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

