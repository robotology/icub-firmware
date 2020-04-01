
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - brief
//   it implements embobj C interface to the use of embot::core 
//

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_CIF_CORE_H_
#define _EMBOT_CIF_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif
 

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct
{  
    eOvoid_fp_void_t    timeinit;
    eOuint64_fp_void_t  timeget;    
} embot_cif_core_Config;
    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
 

extern void embot_cif_core_Init(const embot_cif_core_Config *cfg);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

